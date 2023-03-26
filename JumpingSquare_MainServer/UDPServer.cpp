#include "UDPServer.h"

// UDP ���� ����
SOCKET udpSocket;

// UDP�� ���� ���� �޽��� �ؽ� (first: playerName, second: message)
std::unordered_map<std::string, std::string> toUdpMessageHash;
// �޽��� �ؽ��� ����ȭ�� ��ȣ�ϴ� Mutex
std::mutex udpMessageMutex;

bool CheckClose()
{
	// ���� �޽��� �ҷ�����
	std::string closeMsg = messageData->GetMessageContent(Message::Close);

	// SYSTEM���� ���� �޽����� �ִ��� Ȯ��
	MutexLockHelper lock(&udpMessageMutex);
	std::unordered_map<std::string, std::string>::iterator messageIter = toUdpMessageHash.find("SYSTEM");
	if (messageIter != toUdpMessageHash.end())
	{
		// ���� �޽����� ������ true ��ȯ
		if (!messageIter->second.compare(closeMsg))
		{
			return true;
		}
	}

	// �ش��ϴ� �޽����� ������ false ��ȯ
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
	char buf[BUF_SIZE];			// ���� ����

	int receiveSize = 0;		// ���Ź��� ������ ũ��
	bool connected = true;		// ���� ���� Ŭ���̾�Ʈ�� �ִ��� ����
	bool logDetail = false;		// �ڼ��� �α׸� ǥ������ ����

	// ���� ���� �÷��̾� ������ �ؽ�
	std::unordered_map<std::string, PlayerData> playerDataHash;
	// Ŭ���̾�Ʈ ���� �ּ� ����ü
	SOCKADDR_IN clientSocketAddr;
	// ���� �ּ� ����ü ũ��
	int clientSocketAddrSize;

	// ���� �ּ� ����ü 0���� �ʱ�ȭ
	memset(&clientSocketAddr, 0, sizeof(clientSocketAddr));

	while (connected)
	{
		// ���� ���� �ð� ������
		long long processStart = GetCurrentTimeInMilliSeconds();
		cout << endl;

		// ���� ���� Ȯ��
		if (CheckClose())
			break;

		std::vector<SOCKADDR_IN> clientSockets;			 // �̹� ȸ���� ���Ź��� Ŭ���̾�Ʈ ���� �ּ� ����Ʈ
		clientSocketAddrSize = sizeof(clientSocketAddr); // ���� �ּ� ũ��

		rapidjson::Document arrDoc;						 // �۽��� Json Document
		rapidjson::Value arrayVal;						 // �۽��� Json �迭 ��
		rapidjson::Document newDocument;				 // ���Ź��� Json Document
		rapidjson::Document::AllocatorType& allocator = arrDoc.GetAllocator(); // ������ �߰��� ���� Json Allocator

		// arrayVal�� �迭�� ����
		arrayVal.SetArray();
		// arrDoc�� ������Ʈ�� ����
		arrDoc.SetObject();

		// ���� �� ��ŭ ���Ź޵��� �ݺ�
		for (int i = 0; i < connectedClientCount;)
		{
			// UDP�� ������ ����
			receiveSize = recvfrom(udpSocket, buf, BUF_SIZE, 0, (struct sockaddr*)&clientSocketAddr, &clientSocketAddrSize);
			// �۽��� Ŭ���̾�Ʈ�� �̹� ����Ʈ�� �ִ��� Ȯ��
			if (!CheckDuplicateClient(clientSockets, clientSocketAddr))
			{
				// �ߺ����� �ʾ����� �߰� �� i++
				clientSockets.push_back(clientSocketAddr);
				i++;
			}
			else
				cout << "Duplicated socket" << endl;
			
			// ���� ���� Ȯ��
			if (receiveSize < 0)
			{
				cout << "[UDP]recvfrom() error! " << WSAGetLastError() << endl;
				return;
			}
			// �޽��� ���� ó��
			else if (receiveSize < BUF_SIZE)
			{
				buf[receiveSize] = 0;
				cout << "[UDP]received from " << clientSocketAddr.sin_port << endl;
			}
			// �޽��� ���� �ʰ�
			else
			{
				cout << "[UDP]Too long message" << endl;
				continue;
			}

			// ���� buf�� Json Array��  �߰�
			newDocument.Parse(buf);

			// �÷��̾� ������ �ν��Ͻ� ����
			PlayerData playerFromClient(newDocument);

			// �ش��ϴ� �÷��̾� ������ �ؽÿ��� ã��
			std::unordered_map<std::string, PlayerData>::iterator findIter = playerDataHash.find(playerFromClient.GetPlayerName());

			// �÷��̾� �����Ͱ� �ؽÿ� ���� ��
			if (findIter == playerDataHash.end())
			{
				// ���ο� �÷��̾� �߰�	
				playerDataHash.insert(
					std::unordered_map<std::string, PlayerData>::value_type(playerFromClient.GetPlayerName(), playerFromClient));
			}
			else
			{
				// �÷��̾� ������ �ؽ� ã��
				std::unordered_map <std::string, std::string>::iterator waitingIter = toUdpMessageHash.find(playerFromClient.GetPlayerName());
				// (TCP �����忡�� ����)��� ���� UDP �����Ͱ� ���� ��
				if (waitingIter != toUdpMessageHash.end())
				{
					// ��� ���� ������ ���� (mutex lock)
					MutexLockHelper lock(&udpMessageMutex);
					std::string waitingMessage = waitingIter->second;
					// �޽����� ������ ��û�̸�
					if (!waitingMessage.compare(messageData->GetMessageContent(Message::RespawnRequest)))
					{
						// ���� ���·� �ٲٰ� ��ġ�� ������ ��ġ�� ����
						playerFromClient.SetAlive(true);
						playerFromClient.SetPosition(playerFromClient.GetRespawnPosition());
						// ��ġ ���� �� ��
						newDocument["forceTransform"].SetBool(true);
						// ó���� �޽��� ����
						toUdpMessageHash.erase(playerFromClient.GetPlayerName());
					}
					
					// ����� ����� mutex�� unlock�ȴ�.
				}
			}

			// ��� ���� Ȯ��
			if (playerFromClient.GetPosition().y < mapData->GetLimitY() && playerFromClient.IsAlive())
			{
				// TCP �޽��� ť�� ��� �޽��� ���
				MutexLockHelper lock(&tcpMessageMutex);
				playerFromClient.SetAlive(false);
				toTcpMessageQueue.push({ playerFromClient.GetPlayerName(), messageData->GetMessageContent(Message::Death) });
				cout << "[UDP] \"" << playerFromClient.GetPlayerName() << "\" Player Death Message Queued." << endl;
				// ����� ����� mutex�� unlock�ȴ�.
			}

			// �� ���� Ȯ��
			int map = newDocument["map"].GetInt();
			Vector3 clearPosition = mapData->GetMap(map).GetClearPosition();
			Vector3 clearBoundary = mapData->GetMap(map).GetClearBoundary();
			// Ŭ���� ���� Ȯ��
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
			// �޽����� ���ŵ� ������ �� ��
			ApplyPlayerPositionToDatagram(newDocument, playerFromClient);
			// �۽��� �迭 �����Ϳ� �߰�
			arrayVal.PushBack(newDocument, allocator);
		}
		if (arrayVal.Size() == 0)
			continue;

		// �迭 ���� �ֻ��� ������Ʈ�� �߰�
		arrDoc.AddMember("Items", arrayVal, allocator);
		cout << "[UDP]Position Received" << endl;

		// Json Document�� Json String���� ��ȯ
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		arrDoc.Accept(writer);
		std::string msg = buffer.GetString();
		const char* assembledJson = msg.c_str();

		if(logDetail)
			cout << "[UDP]Assembled Data: " << assembledJson << endl;

		for (int i = 0; i < clientSockets.size(); i++)
			cout << "[UDP]collected socket: " << clientSockets[i].sin_addr.S_un.S_addr << ":" << clientSockets[i].sin_port << endl;

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

		// ����Ŭ �� ó�� �ð� ���
		long long processEnd = GetCurrentTimeInMilliSeconds();
		cout << "[UDP]Process Time 1 cycle: " << (processEnd - processStart) << "ms" << endl;
	}

	cout << "[UDP Disconnect] All clients disconnected." << endl;
}