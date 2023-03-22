#include "UDPServer.h"


DWORD WINAPI DataThreadUDP(LPVOID v)
{
	DWORD dwResult = 0;
	char buf[BUF_SIZE];

	int n = 0;
	bool connected = true;

	stdext::hash_map<std::string, PlayerData> playerDataHash;
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

			// ���� buf�� Json Array��  �߰�
			newDocument.Parse(jsons[jsons.size() - 1].c_str());

			PlayerData player(newDocument);
			std::hash_map<std::string, PlayerData>::iterator findIter = playerDataHash.find(player.GetPlayerName());
			if (findIter == playerDataHash.end())
			{
				playerDataHash.insert(
					std::hash_map<std::string, PlayerData>::value_type(player.GetPlayerName(), player));
			}
			else
			{
				// TODO: �̰� �����۵��ϴ��� Ȯ�� �ʿ�
				// �ֽ� �����ͷ� �÷��̾� ������Ʈ
				findIter->second = player;
			}

			if (player.GetPosition().y < mapData->GetLimitY() && player.IsAlive())
			{
				// ��� ó��
				//newDocument["message"] = std::string(messageData->GetMessageContent(Message::Death));

				messageQueue.push({ player.GetPlayerName(), messageData->GetMessageContent(Message::Death) });
				// TODO : TCP �޽��� ť�� ��� (mutex lock?)
			}

			int map = newDocument["map"].GetInt();
			Vector3 clearPosition = mapData->GetMap(map).GetClearPosition();
			Vector3 clearBoundary = mapData->GetMap(map).GetClearBoundary();
			if (CheckBoundary3D(player.GetPosition(), clearPosition, clearBoundary))
			{
				// Ŭ���� ó��
				// newDocument["message"] = std::string(messageData->GetMessageContent(Message::Clear));
				// TODO : TCP �޽��� ť�� ��� (mutex lock?)
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

		// TODO : ������ ������ ������ �����ص־� ��
		// ���� Json Array�� ��� client�� Broadcast
		for (int i = 0; i < clientSockets.size(); i++)
		{
			send(clientSockets[i], assembledJson, buffer.GetSize(), 0);
		}

		cout << "Position Broadcasted" << endl;
	}

	cout << "[Disconnect] All clients disconnected." << endl;

	return (dwResult);
}