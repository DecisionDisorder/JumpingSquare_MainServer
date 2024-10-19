#include "TCPServer.h"
#include "MutexLock.h"
#include "ServerSettings.h"
#include "PlayerData.h"
#include "Tools.h"
#include "Message.h"
#include "UDPServer.h"

namespace dedi
{
	// ���� ������� Ŭ���̾�Ʈ ���� ť
	std::queue<SOCKET> clientSocketQueue;
	// Ŭ���̾�Ʈ ���� ť�� ����ȭ�� ��ȣ�ϴ� Mutex
	std::mutex socketMutex;

	// TCP�� ���� ���� �޽��� ť (first: playerName, second: message)
	std::queue<std::pair<std::string, std::string>> toTcpMessageQueue;
	// �޽��� ť�� ����ȭ�� ��ȣ�ϴ� Mutex
	std::mutex tcpMessageMutex;

	std::string createTcpMessage(std::string playerName, std::string message, int& bufSize)
	{
		// TCP �޽��� ���� �� Json���� ��ȯ�Ͽ� ��ȯ
		rapidjson::Document doc;
		doc.SetObject();
		doc.AddMember("playerName", playerName, doc.GetAllocator());
		doc.AddMember("message", message, doc.GetAllocator());
		std::string json = getJsonString(doc, bufSize);
		return json;
	}

	void accessComplete(SOCKET& client, std::unordered_map<std::string, SOCKET>& clientSockets)
	{
		// ���� ���� �޽��� �ҷ�����
		std::string message = messageData->GetMessageContent(Message::AccessAccept);
		int bufSize = 0;
		// ���� ���� TCP �޽��� ����
		std::string msg = createTcpMessage("HANDSHAKING", message, bufSize);
		// Ŭ���̾�Ʈ�� ���� ���� �޽��� �۽�
		int s = send(client, msg.c_str(), bufSize, 0);
		cout << "[TCP]Access completed message sended." << "(" << s << ")" << endl;

		// Ŭ���̾�Ʈ�� �ʱ� ���� �޽��� ����
		char name[BUF_SIZE];
		int n = recv(client, name, BUF_SIZE, 0);

		// �޽��� ���� ó��
		if (n == SOCKET_ERROR)
		{
			cout << "[TCP]TCP Socket Error " << WSAGetLastError() << endl;
			return;
		}

		if (n < BUF_SIZE)
			name[n] = 0;

		// ������(|) ������ �ɰ���
		std::vector<std::string> jsons = split(std::string(name), '|');
		// Json Document�� ��ȯ�Ͽ� �÷��̾��� �̸��� ��
		rapidjson::Document nameDoc;
		nameDoc.Parse(jsons[jsons.size() - 1].c_str());
		std::string playerName = nameDoc["playerName"].GetString();
		// ���� ���� �÷��̾� ���� �ؽÿ� �߰�
		clientSockets.insert(make_pair(playerName, client));
	}

	void messageThreadTCP()
	{
		std::unordered_map<std::string, SOCKET> clientSockets;  // �������� Ŭ���̾�Ʈ ���� �ؽ�
		char buf[BUF_SIZE];										// ���� ����
		SOCKET firstSocket = clientSocketQueue.front();			// ù ������ Ŭ���̾�Ʈ
		clientSocketQueue.pop();

		int receivedSize = 0;									// ���Ź��� ������ ũ��
		bool connected = true;									// ���� ���� Ŭ���̾�Ʈ�� �ִ��� ����
		int clientLength = sizeof(firstSocket);					// Ŭ���̾�Ʈ ������ ũ��

		// UDP �ۼ��� ������ ����
		std::thread udpThread(dataThreadUDP);

		// ���� �Ϸ� ó��
		accessComplete(firstSocket, clientSockets);

		while (connected)
		{
			// �ű� ���� �÷��̾� Ȯ��
			while (!clientSocketQueue.empty())
			{
				// Ŭ���̾�Ʈ ���� ť Ȯ�� (mutex lock)
				socketMutex.lock();
				SOCKET client = clientSocketQueue.front();
				clientSocketQueue.pop();
				socketMutex.unlock();

				accessComplete(client, clientSockets);
			}

			// ��� �޽��� ť Ȯ��
			while (!toTcpMessageQueue.empty())
			{
				// TCP �޽��� ť Ȯ��
				tcpMessageMutex.lock();
				std::string playerName = toTcpMessageQueue.front().first;
				std::string message = toTcpMessageQueue.front().second;
				toTcpMessageQueue.pop();
				tcpMessageMutex.unlock();

				// playerName���� ���� �ؽ� �� ã��
				std::unordered_map<std::string, SOCKET>::iterator socketIter = clientSockets.find(playerName);
				SOCKET sock = socketIter->second;
				if (socketIter != clientSockets.end())
				{
					// �޽��� ť���� ã�� �޽����� TCP �޽����� �����Ͽ� Ŭ���̾�Ʈ�� �۽�
					int bufSize = 0;
					std::string json = createTcpMessage(playerName, message, bufSize).c_str();
					send(sock, json.c_str(), bufSize, 0);
				}
			}

			// �������� Ŭ���̾�Ʈ ���� ��ο� ���Ͽ� �ݺ�
			std::unordered_map<std::string, SOCKET>::iterator iter;
			for (iter = clientSockets.begin(); iter != clientSockets.end(); ++iter)
			{
				// Non-Blocking���� Ŭ���̾�Ʈ�κ��� �޽����� �޴´�
				rapidjson::Document newDocument;
				SOCKET client = iter->second;
				receivedSize = recv(client, buf, BUF_SIZE, 0);
				// ���� ���� Ȯ��
				if (receivedSize == SOCKET_ERROR)
				{
					// ���ŵ� �����Ͱ� ���� ���(WOULD_BLOCK)�� �����ϰ�� ���� �޽��� ���
					if (::WSAGetLastError() != WSAEWOULDBLOCK)
					{
						cout << "[TCP][Receive Error] " << WSAGetLastError() << endl;
						break;
					}
					else
						continue;
				}
				// 0�� ���Ź��� ���, ���� ���� ó��
				else if (receivedSize == 0)
				{
					cout << "[TCP]Close Requested" << endl;
					// ���� ���� �ؽÿ��� ����
					clientSockets.erase(iter->first);
					// ���� �� ����
					connectedClientCount--;
					cout << "[TCP][Disconnect] Connected Clients: " << connectedClientCount << endl;
					// ��� ������ ����������, UDP�� ���� �޽����� ������.
					if (connectedClientCount == 0)
					{
						std::string closeMsg = messageData->GetMessageContent(Message::Close);
						MutexLockHelper locker(&udpMessageMutex);
						toUdpMessageHash.insert(std::make_pair("SYSTEM", closeMsg));
						break;
					}

					continue;
				}

				// ���ڿ� ���� ó��
				if (receivedSize < BUF_SIZE)
					buf[receivedSize] = 0;
				else
				{
					cout << "[TCP]Too long message" << endl;
					continue;
				}

				// ������(|) ������ ���ڿ��� ������
				std::vector<std::string> jsons = split(std::string(buf), '|');

				// ���� ���������� ������ ���ڿ��� Json Document�� ��ȯ
				newDocument.Parse(jsons[jsons.size() - 1].c_str());

				// �޽����� �÷��̾� �̸��� �ҷ��´�.
				std::string message = std::string(newDocument["message"].GetString());
				std::string playerName = std::string(newDocument["playerName"].GetString());
				cout << "[Message] " << message << endl;

				// ������ ��û�� ���� ���
				if (message.compare(messageData->GetMessageContent(Message::RespawnRequest)) == 0)
				{
					// ������ ó��
					std::string respawnMsg = std::string(messageData->GetMessageContent(Message::RespawnAccept));
					newDocument["message"].SetString(respawnMsg.data(), static_cast<unsigned int>(respawnMsg.size()), newDocument.GetAllocator());
					// UDP�� ������ ��û (mutex lock)
					MutexLockHelper lock(&udpMessageMutex);
					toUdpMessageHash.insert(
						std::make_pair(
							playerName,
							messageData->GetMessageContent(Message::RespawnRequest)
						)
					);
					// ����� �����鼭 mutex�� unlock
				}

				// ���� �޽��� �۽�
				int bufSize;
				std::string assembledJson = getJsonString(newDocument, bufSize).c_str();
				send(client, assembledJson.c_str(), bufSize, 0);
			}

			// ���� ���� �÷��̾ ������ �ݺ� ����
			if (connectedClientCount == 0)
				break;

			// 0.1�� ��� �� �ݺ�
			cout << "[TCP]Message Checked" << endl;
			Sleep(100);
		}

		cout << "[TCP Disconnect] All clients disconnected." << endl;

		// UDP ������ ���� ���
		udpThread.join();
	}
}