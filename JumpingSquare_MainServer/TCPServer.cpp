#include "TCPServer.h"


void AccessComplete(SOCKET client, stdext::hash_map<std::string, SOCKET>& clientSockets)
{
	const char* message = messageData->GetMessageContent(Message::AccessAccept);
	send(client, message, strlen(message), 0);
	cout << "Access completed message sended." << endl;
	
	char* name;
	int n = recv(client, name, BUF_SIZE, 0);
	if (n < BUF_SIZE)
		name[n] = 0;

	std::vector<std::string> jsons = split(std::string(name), '|');
	rapidjson::Document nameDoc;
	nameDoc.Parse(jsons[jsons.size() - 1].c_str());
	std::string playerName = nameDoc["playerName"].GetString();
	clientSockets.insert(stdext::hash_map<std::string, SOCKET>::value_type(playerName, client));
}

DWORD WINAPI MessageThreadTCP(LPVOID v)
{
	DWORD dwResult = 0;
	stdext::hash_map<std::string, SOCKET> clientSockets;
	char buf[BUF_SIZE];
	//std::vector<SOCKET> clientSockets;
	SOCKET firstSocket = clientSocketQueue.front();
	clientSocketQueue.pop();

	int n = 0;
	bool connected = true;
	int clientLength = sizeof(firstSocket);

	AccessComplete(firstSocket, clientSockets);

	while (connected)
	{
		u_long nonBlockingMode = 1;
		//cout << endl;
		while (!clientSocketQueue.empty())
		{
			SOCKET client = clientSocketQueue.front();
			AccessComplete(client, clientSockets);
			ioctlsocket(client, FIONBIO, &nonBlockingMode); // non-blocking으로 했을 때 recv로는 결과가 안나올 수 있는거 찾아봐야됨
			clientSocketQueue.pop();
		}

		// 모든 메시지 큐 확인
		while (!messageQueue.empty())
		{
			// TCP 메시지 큐 확인
			std::string playerName = messageQueue.front().first;
			std::string message = messageQueue.front().second;
			messageQueue.pop();

			// playerName으로 소켓 해시 값 찾기
			SOCKET sock = clientSockets.find(playerName);

			// TCP 메시지 생성
			rapidjson::Document doc;
			doc.AddMember("playerName", playerName, doc.GetAllocator());
			doc.AddMember("message", message, doc.GetAllocator());
			doc.AddMember("alive", true, doc.GetAllocator()); // 나중에 메시지에 따라 alive 처리 혹은 player data에서 불러오기
			int bufSize;
			const char* json = GetJsonString(doc, bufSize);
			send(sock, json, bufSize, 0);
		}

		stdext::hash_map<std::string, SOCKET>::iterator iter;
		for (iter = clientSockets.begin(); iter != clientSockets.end(); ++iter)
		{

			rapidjson::Document newDocument;
			SOCKET client = iter->second;
			n = recv(client, buf, BUF_SIZE, 0); // TODO : nonblocking 대응
			if (n < 0)
			{
				cout << "[TCP]Close Requested" << endl;
				clientSockets.erase(clientSockets.begin() + i);
				connectedClientCount--;
				cout << "[Disconnect] Connected Clients: " << connectedClientCount << endl;

				if (connectedClientCount == 0)
					break;

				continue;
			}

			if (n < BUF_SIZE)
				buf[n] = 0;
			else
			{
				cout << "Too long message" << endl;
				continue;
			}

			std::vector<std::string> jsons = split(std::string(buf), '|');

			newDocument.Parse(jsons[jsons.size() - 1].c_str());

			std::string message = std::string(newDocument["message"].GetString());
			cout << "[Message] " << message << endl;

			if (message.compare(messageData->GetMessageContent(Message::Close)) == 0)
			{
				//UDP에서 제거해야되면 UDP 데이터큐에 등록?
				continue;
			}
			// TODO: 플레이어를 UDP 데이터에서 읽어와야 됨
			if (player.GetPosition().y < mapData->GetLimitY() && player.IsAlive())
			{
				// 사망 처리
				newDocument["message"] = std::string(messageData->GetMessageContent(Message::Death));
			}
			if (message.compare(messageData->GetMessageContent(Message::RespawnRequest)) == 0)
			{
				// 리스폰 처리
				newDocument["message"] = std::string(messageData->GetMessageContent(Message::RespawnAccept));
				// TODO: 플레이어 리스폰 위치 UDP 에서 읽어와야 됨
				Vector3 respawnPos = player.Respawn();
			}

			int bufSize;
			const char* assembledJson = GetJsonString(newDocument, bufSize);
			send(client, assembledJson, bufSize, 0);
			//cout << "[buf] " << buf << endl;
		}

		cout << "Message Checked" << endl;
		Sleep(100);
	}

	cout << "[Disconnect] All clients disconnected." << endl;

	return (dwResult);
}