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

			// 받은 buf를 Json Array에  추가
			newDocument.Parse(buf);

			PlayerData player(newDocument);

			std::unordered_map<std::string, PlayerData>::iterator findIter = playerDataHash.find(player.GetPlayerName());

			// 플레이어 데이터가 해시에 없을 때
			if (findIter == playerDataHash.end())
			{
				// 새로운 플레이어 추가	
				playerDataHash.insert(
					std::unordered_map<std::string, PlayerData>::value_type(player.GetPlayerName(), player));
			}
			// 클라이언트에서 온 플레이어 데이터 적용
			else
			{
				std::unordered_map <std::string, std::string>::iterator waitingIter = toUdpMessageHash.find(player.GetPlayerName());
				// 대기 중인 데이터가 있을 때
				if (waitingIter != toUdpMessageHash.end())
				{
					// 대기 중인 플레이어 데이터 적용 (mutex lock)
					MutexLockHelper lock(&udpMessageMutex);
					std::string waitingMessage = waitingIter->second;
					if (waitingMessage.compare(messageData->GetMessageContent(Message::RespawnRequest)))
					{
						player.SetPosition(player.GetRespawnPosition());
					}
					
					// 블록을 벗어나면 mutex가 unlock된다.
				}

				// TODO: 이게 정상작동하는지 확인 필요
				// 최신 데이터로 플레이어 업데이트
				findIter->second = player;
				
			}

			if (player.GetPosition().y < mapData->GetLimitY() && player.IsAlive())
			{
				// TCP 메시지 큐에 사망 메시지 등록
				MutexLockHelper lock(&tcpMessageMutex);
				toTcpMessageQueue.push({ player.GetPlayerName(), messageData->GetMessageContent(Message::Death) });
				// 블록을 벗어나면 mutex가 unlock된다.
			}

			int map = newDocument["map"].GetInt();
			Vector3 clearPosition = mapData->GetMap(map).GetClearPosition();
			Vector3 clearBoundary = mapData->GetMap(map).GetClearBoundary();
			if (CheckBoundary3D(player.GetPosition(), clearPosition, clearBoundary))
			{
				// TCP 메시지 큐에 클리어 메시지 등록
				MutexLockHelper lock(&tcpMessageMutex);
				toTcpMessageQueue.push({ player.GetPlayerName(), messageData->GetMessageContent(Message::Clear) });
				// 블록을 벗어나면 mutex가 unlock된다.
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

		// TODO : 데이터 수신한 소켓을 보관해둬야 함
		// 모은 Json Array를 모든 client에 Broadcast
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