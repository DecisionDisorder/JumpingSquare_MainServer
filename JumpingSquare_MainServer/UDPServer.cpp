#include "UDPServer.h"

// UDP 수신 소켓
SOCKET udpSocket;

// UDP로 보낸 내부 메시지 해시 (first: playerName, second: message)
std::unordered_map<std::string, std::string> toUdpMessageHash;
// 메시지 해시의 동기화를 보호하는 Mutex
std::mutex udpMessageMutex;

bool CheckClose()
{
	// 종료 메시지 불러오기
	std::string closeMsg = messageData->GetMessageContent(Message::Close);

	// SYSTEM에서 보낸 메시지가 있는지 확인
	MutexLockHelper lock(&udpMessageMutex);
	std::unordered_map<std::string, std::string>::iterator messageIter = toUdpMessageHash.find("SYSTEM");
	if (messageIter != toUdpMessageHash.end())
	{
		// 종료 메시지가 있으면 true 반환
		if (!messageIter->second.compare(closeMsg))
		{
			return true;
		}
	}

	// 해당하는 메시지가 없으면 false 반환
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
	char buf[BUF_SIZE];			// 수신 버퍼

	int receiveSize = 0;		// 수신받은 데이터 크기
	bool connected = true;		// 접속 중인 클라이언트가 있는지 여부
	bool logDetail = false;		// 자세한 로그를 표기할지 여부

	// 접속 중인 플레이어 데이터 해시
	std::unordered_map<std::string, PlayerData> playerDataHash;
	// 클라이언트 소켓 주소 구조체
	SOCKADDR_IN clientSocketAddr;
	// 소켓 주소 구조체 크기
	int clientSocketAddrSize;

	// 소켓 주소 구조체 0으로 초기화
	memset(&clientSocketAddr, 0, sizeof(clientSocketAddr));

	while (connected)
	{
		// 연산 시작 시간 얻어오기
		long long processStart = GetCurrentTimeInMilliSeconds();
		cout << endl;

		// 종료 여부 확인
		if (CheckClose())
			break;

		std::vector<SOCKADDR_IN> clientSockets;			 // 이번 회차에 수신받은 클라이언트 소켓 주소 리스트
		clientSocketAddrSize = sizeof(clientSocketAddr); // 소켓 주소 크기

		rapidjson::Document arrDoc;						 // 송신할 Json Document
		rapidjson::Value arrayVal;						 // 송신할 Json 배열 값
		rapidjson::Document newDocument;				 // 수신받은 Json Document
		rapidjson::Document::AllocatorType& allocator = arrDoc.GetAllocator(); // 데이터 추가를 위한 Json Allocator

		// arrayVal을 배열로 지정
		arrayVal.SetArray();
		// arrDoc를 오브젝트로 지정
		arrDoc.SetObject();

		// 접속 수 만큼 수신받도록 반복
		for (int i = 0; i < connectedClientCount;)
		{
			// UDP로 데이터 수신
			receiveSize = recvfrom(udpSocket, buf, BUF_SIZE, 0, (struct sockaddr*)&clientSocketAddr, &clientSocketAddrSize);
			// 송신한 클라이언트가 이미 리스트에 있는지 확인
			if (!CheckDuplicateClient(clientSockets, clientSocketAddr))
			{
				// 중복되지 않았으면 추가 후 i++
				clientSockets.push_back(clientSocketAddr);
				i++;
			}
			else
				cout << "Duplicated socket" << endl;
			
			// 수신 오류 확인
			if (receiveSize < 0)
			{
				cout << "[UDP]recvfrom() error! " << WSAGetLastError() << endl;
				return;
			}
			// 메시지 마감 처리
			else if (receiveSize < BUF_SIZE)
			{
				buf[receiveSize] = 0;
				cout << "[UDP]received from " << clientSocketAddr.sin_port << endl;
			}
			// 메시지 길이 초과
			else
			{
				cout << "[UDP]Too long message" << endl;
				continue;
			}

			// 받은 buf를 Json Array에  추가
			newDocument.Parse(buf);

			// 플레이어 데이터 인스턴스 생성
			PlayerData playerFromClient(newDocument);

			// 해당하는 플레이어 데이터 해시에서 찾기
			std::unordered_map<std::string, PlayerData>::iterator findIter = playerDataHash.find(playerFromClient.GetPlayerName());

			// 플레이어 데이터가 해시에 없을 때
			if (findIter == playerDataHash.end())
			{
				// 새로운 플레이어 추가	
				playerDataHash.insert(
					std::unordered_map<std::string, PlayerData>::value_type(playerFromClient.GetPlayerName(), playerFromClient));
			}
			else
			{
				// 플레이어 데이터 해시 찾기
				std::unordered_map <std::string, std::string>::iterator waitingIter = toUdpMessageHash.find(playerFromClient.GetPlayerName());
				// (TCP 스레드에서 보낸)대기 중인 UDP 데이터가 있을 때
				if (waitingIter != toUdpMessageHash.end())
				{
					// 대기 중인 데이터 적용 (mutex lock)
					MutexLockHelper lock(&udpMessageMutex);
					std::string waitingMessage = waitingIter->second;
					// 메시지가 리스폰 요청이면
					if (!waitingMessage.compare(messageData->GetMessageContent(Message::RespawnRequest)))
					{
						// 생존 상태로 바꾸고 위치를 리스폰 위치로 지정
						playerFromClient.SetAlive(true);
						playerFromClient.SetPosition(playerFromClient.GetRespawnPosition());
						// 위치 강제 적 용
						newDocument["forceTransform"].SetBool(true);
						// 처리한 메시지 삭제
						toUdpMessageHash.erase(playerFromClient.GetPlayerName());
					}
					
					// 블록을 벗어나면 mutex가 unlock된다.
				}
			}

			// 사망 조건 확인
			if (playerFromClient.GetPosition().y < mapData->GetLimitY() && playerFromClient.IsAlive())
			{
				// TCP 메시지 큐에 사망 메시지 등록
				MutexLockHelper lock(&tcpMessageMutex);
				playerFromClient.SetAlive(false);
				toTcpMessageQueue.push({ playerFromClient.GetPlayerName(), messageData->GetMessageContent(Message::Death) });
				cout << "[UDP] \"" << playerFromClient.GetPlayerName() << "\" Player Death Message Queued." << endl;
				// 블록을 벗어나면 mutex가 unlock된다.
			}

			// 맵 정보 확인
			int map = newDocument["map"].GetInt();
			Vector3 clearPosition = mapData->GetMap(map).GetClearPosition();
			Vector3 clearBoundary = mapData->GetMap(map).GetClearBoundary();
			// 클리어 기준 확인
			if (CheckBoundary3D(playerFromClient.GetPosition(), clearPosition, clearBoundary))
			{
				// TCP 메시지 큐에 클리어 메시지 등록
				MutexLockHelper lock(&tcpMessageMutex);
				toTcpMessageQueue.push({ playerFromClient.GetPlayerName(), messageData->GetMessageContent(Message::Clear) });
				cout << "[UDP] \"" << playerFromClient.GetPlayerName() << "\" Player Clear Message Queued." << endl;
				// 블록을 벗어나면 mutex가 unlock된다.
			}

			// 최신 데이터로 플레이어 업데이트
			if(findIter != playerDataHash.end())
				findIter->second.ApplyData(playerFromClient);
			// 메시지에 갱신된 데이터 적 용
			ApplyPlayerPositionToDatagram(newDocument, playerFromClient);
			// 송신할 배열 데이터에 추가
			arrayVal.PushBack(newDocument, allocator);
		}
		if (arrayVal.Size() == 0)
			continue;

		// 배열 값을 최상위 오브젝트에 추가
		arrDoc.AddMember("Items", arrayVal, allocator);
		cout << "[UDP]Position Received" << endl;

		// Json Document를 Json String으로 변환
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		arrDoc.Accept(writer);
		std::string msg = buffer.GetString();
		const char* assembledJson = msg.c_str();

		if(logDetail)
			cout << "[UDP]Assembled Data: " << assembledJson << endl;

		for (int i = 0; i < clientSockets.size(); i++)
			cout << "[UDP]collected socket: " << clientSockets[i].sin_addr.S_un.S_addr << ":" << clientSockets[i].sin_port << endl;

		// 모은 Json Array를 모든 client에 Broadcast
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

		// 사이클 당 처리 시간 계산
		long long processEnd = GetCurrentTimeInMilliSeconds();
		cout << "[UDP]Process Time 1 cycle: " << (processEnd - processStart) << "ms" << endl;
	}

	cout << "[UDP Disconnect] All clients disconnected." << endl;
}