#include "UDPServer.h"


DWORD WINAPI DataThreadUDP(LPVOID v)
{
	DWORD dwResult = 0;
	char buf[BUF_SIZE];

	int n = 0;
	bool connected = true;

	SOCKADDR_IN clientSocketAddr;
	int clientSocketAddrSize;

	memset(&clientSocketAddr, 0, sizeof(clientSocketAddr));

	while (connected)
	{
		clientSocketAddrSize = sizeof(clientSocketAddr);

		rapidjson::Document arrDoc;
		rapidjson::Value arrayVal;
		rapidjson::Document newDocument;
		rapidjson::Document::AllocatorType& allocator = arrDoc.GetAllocator();

		arrayVal.SetArray();
		arrDoc.SetObject();

		for (int i = 0; i < connectedClientCount; i++)
		{
			n = recvfrom(udpSocket, buf, BUF_SIZE, 0, (struct sockaddr*)&clientSocketAddr, &clientSocketAddrSize);
			
			if (n < BUF_SIZE)
				buf[n] = 0;
			else
			{
				cout << "Too long message" << endl;
				continue;
			}

			std::vector<std::string> jsons = split(std::string(buf), '|');

			// 받은 buf를 Json Array에  추가
			newDocument.Parse(jsons[jsons.size() - 1].c_str());

			PlayerData player(newDocument);
			playerDataList.push_back(player);

			if (player.GetPosition().y < mapData->GetLimitY() && player.IsAlive())
			{
				// 사망 처리
				//newDocument["message"] = std::string(messageData->GetMessageContent(Message::Death));
				// TODO : TCP 메시지 큐에 등록 (mutex lock?)
			}

			int map = newDocument["map"].GetInt();
			Vector3 clearPosition = mapData->GetMap(map).GetClearPosition();
			Vector3 clearBoundary = mapData->GetMap(map).GetClearBoundary();
			if (CheckBoundary3D(player.GetPosition(), clearPosition, clearBoundary))
			{
				// 클리어 처리
				// newDocument["message"] = std::string(messageData->GetMessageContent(Message::Clear));
				// TODO : TCP 메시지 큐에 등록 (mutex lock?)
			}

			arrayVal.PushBack(newDocument, allocator);
			//cout << "[buf] " << buf << endl;
		}
		if (arrayVal.Size() == 0)
			continue;

		arrDoc.AddMember("Items", arrayVal, allocator);
		cout << "Position Received" << endl;

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		arrDoc.Accept(writer);

		std::string msg = buffer.GetString();
		msg.append("|");

		const char* assembledJson = msg.c_str();

		cout << "Assembled Data: " << assembledJson << endl;

		// TODO : 데이터 수신한 소켓을 보관해둬야 함
		// 모은 Json Array를 모든 client에 Broadcast
		for (int i = 0; i < clientSockets.size(); i++)
		{
			send(clientSockets[i], assembledJson, buffer.GetSize(), 0);
		}

		cout << "Position Broadcasted" << endl;
	}

	cout << "[Disconnect] All clients disconnected." << endl;

	return (dwResult);
}