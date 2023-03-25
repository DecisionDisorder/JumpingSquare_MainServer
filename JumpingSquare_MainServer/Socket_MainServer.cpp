#include "Socket_MainServer.h"


int main()
{
	WORD		wVersionRequested;		// 
	WSADATA		wsaData;				// Windows에서 소켓 초기화 정보를 저장하기 위한 구조체
	SOCKADDR_IN tcpSocketAddr;
	SOCKADDR_IN clientAddr;				
	SOCKADDR_IN udpSocketAddr;
	int			err;
	int			byteSent;

	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::cout.tie(NULL);

	// Windows Socket API 버전 설정 및 초기화
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	
	// 초기화 오류 출력
	if (err != 0) {
		cout << "WSAStartup error " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// TCP 소켓 ip:port 정보 설정
	tcpSocketAddr.sin_family = AF_INET;							// Address Family Internet
	tcpSocketAddr.sin_port = htons(TCP_PORT);			// 수신할 Port #
	tcpSocketAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);	// 타켓 IP


	SOCKET tcpListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // TCP 소켓 생성
	if (tcpListenSocket == INVALID_SOCKET)
	{
		cout << "TCP Socket Error " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	u_long on = 1;
	if (ioctlsocket(tcpListenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 1;

	int x = bind(tcpListenSocket, reinterpret_cast<SOCKADDR*>(&tcpSocketAddr), sizeof(tcpSocketAddr));
	if (x == SOCKET_ERROR)
	{
		cout << "[TCP Port]Binding failed. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	memset(&udpSocketAddr, 0, sizeof(udpSocketAddr));

	udpSocketAddr.sin_family = AF_INET;							// Address Family Internet
	udpSocketAddr.sin_port = htons(UDP_PORT);					// 수신할 Port #
	udpSocketAddr.sin_addr.S_un.S_addr = inet_addr(IP_ADDRESS);	// 타켓 IP

	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSocket == INVALID_SOCKET)
	{
		cout << "Game Socket Error " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	int g = bind(udpSocket, (sockaddr*)&udpSocketAddr, sizeof(udpSocketAddr));
	if (g == SOCKET_ERROR)
	{
		cout << "[Game Port]Binding failed. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// Map 및 Message 데이터 파일 읽기
	mapData = MapData::ReadMapDataFromFile();
	messageData = Message::ReadDataFromFile();

	listen(tcpListenSocket, 5);
	std::thread* tcpThread = nullptr;
	bool nextWaiting = true;

	while (true)
	{
		if(nextWaiting)
			cout << "Waiting for client..." << endl;

		int xx = sizeof(clientAddr);
		SOCKET clientSocket = accept(tcpListenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &xx);

		if (clientSocket == INVALID_SOCKET)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				nextWaiting = false;
				continue;
			}

			cout << "[Listen InvalidSocket] " << WSAGetLastError() << endl;
			break;
		}

		cout << "Connection established. New socket num is " << clientSocket << endl;

		// 일단은 처음 접속만 새로운 방 개설로 처리함.
		if (connectedClientCount == 0)
		{
			clientSocketQueue.push(clientSocket);
			if(tcpThread == nullptr)
				tcpThread = new std::thread(MessageThreadTCP);
		}
		else
		{
			// 소켓 추가에 대한 mutex lock
			MutexLockHelper lock(&socketMutex);
			clientSocketQueue.push(clientSocket);
			// 블록을 벗어나면서 mutex unlock
		}
		connectedClientCount++;
		cout << "[Connect] Connected Clients: " << connectedClientCount << endl;
		nextWaiting = true;
	}

	delete tcpThread;
	closesocket(tcpListenSocket);
	closesocket(udpSocket);
	WSACleanup();

	return 0;
}