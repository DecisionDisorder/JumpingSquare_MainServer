#include "TCPServer.h"
#include "MutexLock.h"
#include "ServerSettings.h"
#include "PlayerData.h"
#include "Tools.h"
#include "Message.h"
#include "UDPServer.h"

namespace dedi
{
	// 접속 대기중인 클라이언트 소켓 큐
	std::queue<SOCKET> clientSocketQueue;
	// 클라이언트 소켓 큐의 동기화를 보호하는 Mutex
	std::mutex socketMutex;

	// TCP로 보낸 내부 메시지 큐 (first: playerName, second: message)
	std::queue<std::pair<std::string, std::string>> toTcpMessageQueue;
	// 메시지 큐의 동기화를 보호하는 Mutex
	std::mutex tcpMessageMutex;

	std::string createTcpMessage(std::string playerName, std::string message, int& bufSize)
	{
		// TCP 메시지 생성 후 Json으로 변환하여 반환
		rapidjson::Document doc;
		doc.SetObject();
		doc.AddMember("playerName", playerName, doc.GetAllocator());
		doc.AddMember("message", message, doc.GetAllocator());
		std::string json = getJsonString(doc, bufSize);
		return json;
	}

	void accessComplete(SOCKET& client, std::unordered_map<std::string, SOCKET>& clientSockets)
	{
		// 접속 승인 메시지 불러오기
		std::string message = messageData->GetMessageContent(Message::AccessAccept);
		int bufSize = 0;
		// 접속 승인 TCP 메시지 생성
		std::string msg = createTcpMessage("HANDSHAKING", message, bufSize);
		// 클라이언트에 접속 승인 메시지 송신
		int s = send(client, msg.c_str(), bufSize, 0);
		cout << "[TCP]Access completed message sended." << "(" << s << ")" << endl;

		// 클라이언트의 초기 정보 메시지 수신
		char name[BUF_SIZE];
		int n = recv(client, name, BUF_SIZE, 0);

		// 메시지 오류 처리
		if (n == SOCKET_ERROR)
		{
			cout << "[TCP]TCP Socket Error " << WSAGetLastError() << endl;
			return;
		}

		if (n < BUF_SIZE)
			name[n] = 0;

		// 구분자(|) 단위로 쪼개기
		std::vector<std::string> jsons = split(std::string(name), '|');
		// Json Document로 변환하여 플레이어의 이름을 얻어냄
		rapidjson::Document nameDoc;
		nameDoc.Parse(jsons[jsons.size() - 1].c_str());
		std::string playerName = nameDoc["playerName"].GetString();
		// 접속 중인 플레이어 소켓 해시에 추가
		clientSockets.insert(make_pair(playerName, client));
	}

	void messageThreadTCP()
	{
		std::unordered_map<std::string, SOCKET> clientSockets;  // 접속중인 클라이언트 소켓 해시
		char buf[BUF_SIZE];										// 수신 버퍼
		SOCKET firstSocket = clientSocketQueue.front();			// 첫 접속한 클라이언트
		clientSocketQueue.pop();

		int receivedSize = 0;									// 수신받은 데이터 크기
		bool connected = true;									// 접속 중인 클라이언트가 있는지 여부
		int clientLength = sizeof(firstSocket);					// 클라이언트 소켓의 크기

		// UDP 송수신 스레드 시작
		std::thread udpThread(dataThreadUDP);

		// 접속 완료 처리
		accessComplete(firstSocket, clientSockets);

		while (connected)
		{
			// 신규 접속 플레이어 확인
			while (!clientSocketQueue.empty())
			{
				// 클라이언트 소켓 큐 확인 (mutex lock)
				socketMutex.lock();
				SOCKET client = clientSocketQueue.front();
				clientSocketQueue.pop();
				socketMutex.unlock();

				accessComplete(client, clientSockets);
			}

			// 모든 메시지 큐 확인
			while (!toTcpMessageQueue.empty())
			{
				// TCP 메시지 큐 확인
				tcpMessageMutex.lock();
				std::string playerName = toTcpMessageQueue.front().first;
				std::string message = toTcpMessageQueue.front().second;
				toTcpMessageQueue.pop();
				tcpMessageMutex.unlock();

				// playerName으로 소켓 해시 값 찾기
				std::unordered_map<std::string, SOCKET>::iterator socketIter = clientSockets.find(playerName);
				SOCKET sock = socketIter->second;
				if (socketIter != clientSockets.end())
				{
					// 메시지 큐에서 찾은 메시지를 TCP 메시지로 생성하여 클라이언트에 송신
					int bufSize = 0;
					std::string json = createTcpMessage(playerName, message, bufSize).c_str();
					send(sock, json.c_str(), bufSize, 0);
				}
			}

			// 접속중인 클라이언트 소켓 모두에 대하여 반복
			std::unordered_map<std::string, SOCKET>::iterator iter;
			for (iter = clientSockets.begin(); iter != clientSockets.end(); ++iter)
			{
				// Non-Blocking으로 클라이언트로부터 메시지를 받는다
				rapidjson::Document newDocument;
				SOCKET client = iter->second;
				receivedSize = recv(client, buf, BUF_SIZE, 0);
				// 소켓 에러 확인
				if (receivedSize == SOCKET_ERROR)
				{
					// 수신된 데이터가 없는 경우(WOULD_BLOCK)를 제외하고는 오류 메시지 출력
					if (::WSAGetLastError() != WSAEWOULDBLOCK)
					{
						cout << "[TCP][Receive Error] " << WSAGetLastError() << endl;
						break;
					}
					else
						continue;
				}
				// 0을 수신받은 경우, 접속 종료 처리
				else if (receivedSize == 0)
				{
					cout << "[TCP]Close Requested" << endl;
					// 접속 소켓 해시에서 제거
					clientSockets.erase(iter->first);
					// 접속 수 차감
					connectedClientCount--;
					cout << "[TCP][Disconnect] Connected Clients: " << connectedClientCount << endl;
					// 모두 접속을 해재했으면, UDP에 종료 메시지를 보낸다.
					if (connectedClientCount == 0)
					{
						std::string closeMsg = messageData->GetMessageContent(Message::Close);
						MutexLockHelper locker(&udpMessageMutex);
						toUdpMessageHash.insert(std::make_pair("SYSTEM", closeMsg));
						break;
					}

					continue;
				}

				// 문자열 마감 처리
				if (receivedSize < BUF_SIZE)
					buf[receivedSize] = 0;
				else
				{
					cout << "[TCP]Too long message" << endl;
					continue;
				}

				// 구분자(|) 단위로 문자열을 나눈다
				std::vector<std::string> jsons = split(std::string(buf), '|');

				// 가장 마지막으로 수신한 문자열만 Json Document로 변환
				newDocument.Parse(jsons[jsons.size() - 1].c_str());

				// 메시지와 플레이어 이름을 불러온다.
				std::string message = std::string(newDocument["message"].GetString());
				std::string playerName = std::string(newDocument["playerName"].GetString());
				cout << "[Message] " << message << endl;

				// 리스폰 요청이 들어온 경우
				if (message.compare(messageData->GetMessageContent(Message::RespawnRequest)) == 0)
				{
					// 리스폰 처리
					std::string respawnMsg = std::string(messageData->GetMessageContent(Message::RespawnAccept));
					newDocument["message"].SetString(respawnMsg.data(), static_cast<unsigned int>(respawnMsg.size()), newDocument.GetAllocator());
					// UDP에 리스폰 요청 (mutex lock)
					MutexLockHelper lock(&udpMessageMutex);
					toUdpMessageHash.insert(
						std::make_pair(
							playerName,
							messageData->GetMessageContent(Message::RespawnRequest)
						)
					);
					// 블록이 끝나면서 mutex가 unlock
				}

				// 답장 메시지 송신
				int bufSize;
				std::string assembledJson = getJsonString(newDocument, bufSize).c_str();
				send(client, assembledJson.c_str(), bufSize, 0);
			}

			// 접속 중인 플레이어가 없으면 반복 종료
			if (connectedClientCount == 0)
				break;

			// 0.1초 대기 후 반복
			cout << "[TCP]Message Checked" << endl;
			Sleep(100);
		}

		cout << "[TCP Disconnect] All clients disconnected." << endl;

		// UDP 스레드 종료 대기
		udpThread.join();
	}
}