#include "UDPServer.h"

SOCKET udpSocket;

std::unordered_map<std::string, std::string> toUdpMessageHash;
std::mutex udpMessageMutex;

bool CheckClose()
{
	std::string closeMsg = messageData->GetMessageContent(Message::Close);

	MutexLockHelper lock(&udpMessageMutex);
	std::unordered_map<std::string, std::string>::iterator messageIter = toUdpMessageHash.find("SYSTEM");
	if (messageIter != toUdpMessageHash.end())
	{
		if (!messageIter->first.compare("SYSTEM") && !messageIter->second.compare(closeMsg))
		{
			return true;
		}
	}

	return false;
}

void DataThreadUDP()
{
	DWORD dwResult = 0;
	char buf[BUF_SIZE];

	int n = 0;
	bool connected = true;

	std::unordered_map<std::string, PlayerData> playerDataHash;
	SOCKADDR_IN clientSocketAddr;
	int clientSocketAddrSize;

	memset(&clientSocketAddr, 0, sizeof(clientSocketAddr));

	while (connected)
	{
		if (CheckClose())
			break;

		std::vector<SOCKADDR_IN*> clientSockets;
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
			clientSockets.push_back(&clientSocketAddr);
			
			if (n < 0)
			{
				cout << "recvfrom() error! " << WSAGetLastError() << endl;
				return;
			}
			else if (n < BUF_SIZE)
				buf[n] = 0;
			else
			{
				cout << "Too long message" << endl;
				continue;
			}

			// ���� buf�� Json Array��  �߰�
			newDocument.Parse(buf);

			PlayerData player(newDocument);

			std::unordered_map<std::string, PlayerData>::iterator findIter = playerDataHash.find(player.GetPlayerName());

			// �÷��̾� �����Ͱ� �ؽÿ� ���� ��
			if (findIter == playerDataHash.end())
			{
				// ���ο� �÷��̾� �߰�	
				playerDataHash.insert(
					std::unordered_map<std::string, PlayerData>::value_type(player.GetPlayerName(), player));
			}
			// Ŭ���̾�Ʈ���� �� �÷��̾� ������ ����
			else
			{
				std::unordered_map <std::string, std::string>::iterator waitingIter = toUdpMessageHash.find(player.GetPlayerName());
				// ��� ���� �����Ͱ� ���� ��
				if (waitingIter != toUdpMessageHash.end())
				{
					// ��� ���� �÷��̾� ������ ���� (mutex lock)
					MutexLockHelper lock(&udpMessageMutex);
					std::string waitingMessage = waitingIter->second;
					if (waitingMessage.compare(messageData->GetMessageContent(Message::RespawnRequest)))
					{
						player.SetPosition(player.GetRespawnPosition());
					}
					
					// ����� ����� mutex�� unlock�ȴ�.
				}

				// TODO: �̰� �����۵��ϴ��� Ȯ�� �ʿ�
				// �ֽ� �����ͷ� �÷��̾� ������Ʈ
				findIter->second = player;
				
			}

			if (player.GetPosition().y < mapData->GetLimitY() && player.IsAlive())
			{
				// TCP �޽��� ť�� ��� �޽��� ���
				MutexLockHelper lock(&tcpMessageMutex);
				toTcpMessageQueue.push({ player.GetPlayerName(), messageData->GetMessageContent(Message::Death) });
				// ����� ����� mutex�� unlock�ȴ�.
			}

			int map = newDocument["map"].GetInt();
			Vector3 clearPosition = mapData->GetMap(map).GetClearPosition();
			Vector3 clearBoundary = mapData->GetMap(map).GetClearBoundary();
			if (CheckBoundary3D(player.GetPosition(), clearPosition, clearBoundary))
			{
				// TCP �޽��� ť�� Ŭ���� �޽��� ���
				MutexLockHelper lock(&tcpMessageMutex);
				toTcpMessageQueue.push({ player.GetPlayerName(), messageData->GetMessageContent(Message::Clear) });
				// ����� ����� mutex�� unlock�ȴ�.
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

		const char* assembledJson = msg.c_str();

		cout << "Assembled Data: " << assembledJson << endl;

		// TODO : ������ ������ ������ �����ص־� ��
		// ���� Json Array�� ��� client�� Broadcast
		for (int i = 0; i < clientSockets.size(); i++)
		{
			int sendSize = sendto(udpSocket, assembledJson, msg.size(), 0, (struct sockaddr*)clientSockets[i], sizeof(*(clientSockets[i])));
			if (sendSize != msg.size())
			{
				cout << "[UDP] sendto error occured!" << endl;
			}
			else
			{
				cout << "[UDP] Send message to " << inet_ntoa(clientSockets[i]->sin_addr) << ":" << clientSockets[i]->sin_port << endl;
			}
		}

		cout << "Position Broadcasted" << endl;
	}

	cout << "[UDP Disconnect] All clients disconnected." << endl;
}