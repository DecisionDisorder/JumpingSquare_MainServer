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

bool CheckDuplicateClient(std::vector<SOCKADDR_IN>& clientSockets, SOCKADDR_IN newClient)
{
	for (int i = 0; i < clientSockets.size(); i++)
	{
		if (clientSockets[i].sin_addr.S_un.S_addr == newClient.sin_addr.S_un.S_addr 
			&& clientSockets[i].sin_port == newClient.sin_port)
		{
			return true;
		}
	}
	
	return false;
}

void ApplyPlayerPositionToDatagram(rapidjson::Document& doc, PlayerData player)
{
	doc["positionX"].SetDouble(player.GetPosition().x);
	doc["positionY"].SetDouble(player.GetPosition().y);
	doc["positionZ"].SetDouble(player.GetPosition().z);
	doc["alive"].SetBool(player.IsAlive());
}

void DataThreadUDP()
{
	DWORD dwResult = 0;
	char buf[BUF_SIZE];

	int n = 0;
	bool connected = true;
	bool logDetail = false;

	std::unordered_map<std::string, PlayerData> playerDataHash;
	SOCKADDR_IN clientSocketAddr;
	int clientSocketAddrSize;

	memset(&clientSocketAddr, 0, sizeof(clientSocketAddr));

	while (connected)
	{
		long long processStart = GetCurrentTimeInMilliSeconds();
		cout << endl;

		if (CheckClose())
			break;

		std::vector<SOCKADDR_IN> clientSockets;
		clientSocketAddrSize = sizeof(clientSocketAddr);

		rapidjson::Document arrDoc;
		rapidjson::Value arrayVal;
		rapidjson::Document newDocument;
		rapidjson::Document::AllocatorType& allocator = arrDoc.GetAllocator();

		arrayVal.SetArray();
		arrDoc.SetObject();

		for (int i = 0; i < connectedClientCount;)
		{
			n = recvfrom(udpSocket, buf, BUF_SIZE, 0, (struct sockaddr*)&clientSocketAddr, &clientSocketAddrSize);
			if (!CheckDuplicateClient(clientSockets, clientSocketAddr))
			{
				clientSockets.push_back(clientSocketAddr);
				i++;
			}
			else
				cout << "Duplicated socket" << endl;
			
			if (n < 0)
			{
				cout << "[UDP]recvfrom() error! " << WSAGetLastError() << endl;
				return;
			}
			else if (n < BUF_SIZE)
			{
				buf[n] = 0;
				cout << "[UDP]received from " << clientSocketAddr.sin_port << endl;
			}
			else
			{
				cout << "[UDP]Too long message" << endl;
				continue;
			}

			// ���� buf�� Json Array��  �߰�
			newDocument.Parse(buf);

			PlayerData playerFromClient(newDocument);

			std::unordered_map<std::string, PlayerData>::iterator findIter = playerDataHash.find(playerFromClient.GetPlayerName());

			// �÷��̾� �����Ͱ� �ؽÿ� ���� ��
			if (findIter == playerDataHash.end())
			{
				// ���ο� �÷��̾� �߰�	
				playerDataHash.insert(
					std::unordered_map<std::string, PlayerData>::value_type(playerFromClient.GetPlayerName(), playerFromClient));
			}
			// Ŭ���̾�Ʈ���� �� �÷��̾� ������ ����
			else
			{
				std::unordered_map <std::string, std::string>::iterator waitingIter = toUdpMessageHash.find(playerFromClient.GetPlayerName());
				// ��� ���� �����Ͱ� ���� ��
				if (waitingIter != toUdpMessageHash.end())
				{
					// ��� ���� �÷��̾� ������ ���� (mutex lock)
					MutexLockHelper lock(&udpMessageMutex);
					std::string waitingMessage = waitingIter->second;
					if (!waitingMessage.compare(messageData->GetMessageContent(Message::RespawnRequest)))
					{
						playerFromClient.SetAlive(true);
						playerFromClient.SetPosition(playerFromClient.GetRespawnPosition());
						newDocument["forceTransform"].SetBool(true);
						toUdpMessageHash.erase(playerFromClient.GetPlayerName());
					}
					
					// ����� ����� mutex�� unlock�ȴ�.
				}
			}

			if (playerFromClient.GetPosition().y < mapData->GetLimitY() && playerFromClient.IsAlive())
			{
				// TCP �޽��� ť�� ��� �޽��� ���
				MutexLockHelper lock(&tcpMessageMutex);
				playerFromClient.SetAlive(false);
				toTcpMessageQueue.push({ playerFromClient.GetPlayerName(), messageData->GetMessageContent(Message::Death) });
				cout << "[UDP] \"" << playerFromClient.GetPlayerName() << "\" Player Death Message Queued." << endl;
				// ����� ����� mutex�� unlock�ȴ�.
			}

			int map = newDocument["map"].GetInt();
			Vector3 clearPosition = mapData->GetMap(map).GetClearPosition();
			Vector3 clearBoundary = mapData->GetMap(map).GetClearBoundary();
			if (CheckBoundary3D(playerFromClient.GetPosition(), clearPosition, clearBoundary))
			{
				// TCP �޽��� ť�� Ŭ���� �޽��� ���
				MutexLockHelper lock(&tcpMessageMutex);
				toTcpMessageQueue.push({ playerFromClient.GetPlayerName(), messageData->GetMessageContent(Message::Clear) });
				cout << "[UDP] \"" << playerFromClient.GetPlayerName() << "\" Player Clear Message Queued." << endl;
				// ����� ����� mutex�� unlock�ȴ�.
			}

			// �ֽ� �����ͷ� �÷��̾� ������Ʈ
			if(findIter != playerDataHash.end())
				findIter->second.ApplyData(playerFromClient);
			ApplyPlayerPositionToDatagram(newDocument, playerFromClient);
			arrayVal.PushBack(newDocument, allocator);
			//cout << "[buf] " << buf << endl;
		}
		if (arrayVal.Size() == 0)
			continue;

		arrDoc.AddMember("Items", arrayVal, allocator);
		cout << "[UDP]Position Received" << endl;

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		arrDoc.Accept(writer);

		std::string msg = buffer.GetString();

		const char* assembledJson = msg.c_str();

		if(logDetail)
			cout << "[UDP]Assembled Data: " << assembledJson << endl;

		//clientSockets.erase(std::unique(clientSockets.begin(), clientSockets.end()), clientSockets.end());

		for (int i = 0; i < clientSockets.size(); i++)
			cout << "[UDP]collected socket: " << clientSockets[i].sin_addr.S_un.S_addr << ":" << clientSockets[i].sin_port << endl;

		// TODO : ������ ������ ������ �����ص־� ��
		// ���� Json Array�� ��� client�� Broadcast
		for (int i = 0; i < clientSockets.size(); i++)
		{
			struct sockaddr* clientaddr = (struct sockaddr*)&clientSockets[i];
			int sendSize = sendto(udpSocket, assembledJson, msg.size(), 0, clientaddr, sizeof(*clientaddr));
			if (sendSize != msg.size())
			{
				cout << "[UDP] sendto error occured!" << endl;
			}
			else
			{
				cout << "[UDP] Send datagram to " << inet_ntoa(clientSockets[i].sin_addr) << ":" << clientSockets[i].sin_port << endl;
			}
		}
		cout << "[UDP]Position Broadcasted" << endl;

		long long processEnd = GetCurrentTimeInMilliSeconds();
		cout << "[UDP]Process Time 1 cycle: " << (processEnd - processStart) << "ms" << endl;
	}

	cout << "[UDP Disconnect] All clients disconnected." << endl;
}