#include "Socket_MainServer.h"


int main()
{
	WORD		wVersionRequested;		// 
	WSADATA		wsaData;				// Windows���� ���� �ʱ�ȭ ������ �����ϱ� ���� ����ü
	SOCKADDR_IN tcpSocketAddr;
	SOCKADDR_IN clientAddr;				
	SOCKADDR_IN udpSocketAddr;
	int			err;
	int			byteSent;

	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::cout.tie(NULL);

	// Windows Socket API ���� ���� �� �ʱ�ȭ
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	
	// �ʱ�ȭ ���� ���
	if (err != 0) {
		cout << "WSAStartup error " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// TCP ���� ip:port ���� ����
	tcpSocketAddr.sin_family = AF_INET;							// Address Family Internet
	tcpSocketAddr.sin_port = htons(TCP_PORT);			// ������ Port #
	tcpSocketAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);	// Ÿ�� IP


	SOCKET tcpListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // TCP ���� ����
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
	udpSocketAddr.sin_port = htons(UDP_PORT);					// ������ Port #
	udpSocketAddr.sin_addr.S_un.S_addr = inet_addr(IP_ADDRESS);	// Ÿ�� IP

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

	// Map �� Message ������ ���� �б�
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

		// �ϴ��� ó�� ���Ӹ� ���ο� �� ������ ó����.
		if (connectedClientCount == 0)
		{
			clientSocketQueue.push(clientSocket);
			if(tcpThread == nullptr)
				tcpThread = new std::thread(MessageThreadTCP);
		}
		else
		{
			// ���� �߰��� ���� mutex lock
			MutexLockHelper lock(&socketMutex);
			clientSocketQueue.push(clientSocket);
			// ����� ����鼭 mutex unlock
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