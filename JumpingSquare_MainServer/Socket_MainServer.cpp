#include "Socket_MainServer.h"
#include "MutexLock.h"
#include "ServerSettings.h"
#include "Vector3.h"
#include "PlayerData.h"
#include "Tools.h"
#include "MapData.h"
#include "Message.h"
#include "TCPServer.h"
#include "UDPServer.h"

using namespace dedi;

int main()
{
	WORD		wVersionRequested;		// Winsock ���� ����
	WSADATA		wsaData;				// Windows���� ���� �ʱ�ȭ ������ �����ϱ� ���� ����ü
	SOCKADDR_IN tcpSocketAddr;			// TCP ���� ���� �ּ� ����ü
	SOCKADDR_IN clientAddr;				// TCP Ŭ���̾�Ʈ �ּ� ����ü
	SOCKADDR_IN udpSocketAddr;			// UDP ���� ���� �ּ� ����ü
	int			err;					// ���� �ڵ� ���� ����

	// cin, cout ������� ���̱� ���� ó��
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

	// TCP ���� ����
	SOCKET tcpListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	// ���� ������ ���� �ʾ��� �� ���� �޽��� ��� �� ����
	if (tcpListenSocket == INVALID_SOCKET)
	{
		cout << "TCP Socket Error " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// Non-Blocking �������� ����
	u_long on = 1;
	if (ioctlsocket(tcpListenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 1;

	// TCP ������ IP/Port ���ε�
	int x = bind(tcpListenSocket, reinterpret_cast<SOCKADDR*>(&tcpSocketAddr), sizeof(tcpSocketAddr));
	if (x == SOCKET_ERROR)
	{
		cout << "[TCP Port]Binding failed. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// UDP ���� �ּ� ����ü �ʱ�ȭ
	memset(&udpSocketAddr, 0, sizeof(udpSocketAddr));

	udpSocketAddr.sin_family = AF_INET;								// Address Family Internet
	udpSocketAddr.sin_port = htons(UDP_PORT);				// ������ Port #
	udpSocketAddr.sin_addr.S_un.S_addr = inet_addr(IP_ADDRESS);	// Ÿ�� IP

	// UDP ���� ����
	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	// ���� ������ ���� �ʾ��� �� ���� �޽��� ��� �� ����
	if (udpSocket == INVALID_SOCKET)
	{
		cout << "Game Socket Error " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// UDP ���� IP/Port ���ε�
	int g = bind(udpSocket, (sockaddr*)&udpSocketAddr, sizeof(udpSocketAddr));
	if (g == SOCKET_ERROR)
	{
		cout << "[Game Port]Binding failed. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// Map �� Message ������ ���� �б�
	mapData = MapData::readMapDataFromFile();
	messageData = Message::readDataFromFile();

	// TCP Listen  ����
	listen(tcpListenSocket, 5);
	std::thread* tcpThread = nullptr;   // TCP ��� ���� ������ ������
	bool nextWaiting = true;			// Ŭ���̾�Ʈ ��� �޽��� ��� ����

	while (true)
	{
		if(nextWaiting)
			cout << "Waiting for client..." << endl;

		// TCP Ŭ���̾�Ʈ Accept
		int xx = sizeof(clientAddr);
		SOCKET clientSocket = accept(tcpListenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &xx);

		// ���Ͽ� ������ �ִ��� Ȯ��
		if (clientSocket == INVALID_SOCKET)
		{
			// ���ŵ� �����Ͱ� ���� ��� (WOULD_BLOCK) �ٽ� accept �ݺ�
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				nextWaiting = false;
				continue;
			}

			// �׿��� ���, ���� ��� �� �ݺ� ����
			cout << "[Listen InvalidSocket] " << WSAGetLastError() << endl;
			break;
		}

		// ���ο� ���� ������ ���� �޽��� ���
		cout << "Connection established. New socket num is " << clientSocket << endl;

		// �ϴ��� ó�� ���Ӹ� ���ο� �� ������ ó����.
		if (connectedClientCount == 0)
		{
			clientSocketQueue.push(clientSocket);
			if(tcpThread == nullptr)
				tcpThread = new std::thread(messageThreadTCP);
		}
		else
		{
			// ���� �߰��� ���� mutex lock
			MutexLockHelper lock(&socketMutex);
			clientSocketQueue.push(clientSocket);
			// ����� ����鼭 mutex unlock
		}
		// ���� ���ӷ� ����
		connectedClientCount++;
		cout << "[Connect] Connected Clients: " << connectedClientCount << endl;
		nextWaiting = true;
	}

	// thread ����
	delete tcpThread;
	// ���� �ݱ�
	closesocket(tcpListenSocket);
	closesocket(udpSocket);
	WSACleanup();

	return 0;
}