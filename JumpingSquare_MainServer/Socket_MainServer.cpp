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
	WORD		wVersionRequested;		// Winsock 버전 지정
	WSADATA		wsaData;				// Windows에서 소켓 초기화 정보를 저장하기 위한 구조체
	SOCKADDR_IN tcpSocketAddr;			// TCP 수신 소켓 주소 구조체
	SOCKADDR_IN clientAddr;				// TCP 클라이언트 주소 구조체
	SOCKADDR_IN udpSocketAddr;			// UDP 수신 소켓 주소 구조체
	int			err;					// 오류 코드 보관 변수

	// cin, cout 오버헤드 줄이기 위한 처리
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

	// TCP 소켓 생성
	SOCKET tcpListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	// 소켓 생성이 되지 않았을 때 오류 메시지 출력 후 종료
	if (tcpListenSocket == INVALID_SOCKET)
	{
		cout << "TCP Socket Error " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// Non-Blocking 소켓으로 설정
	u_long on = 1;
	if (ioctlsocket(tcpListenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 1;

	// TCP 소켓의 IP/Port 바인드
	int x = bind(tcpListenSocket, reinterpret_cast<SOCKADDR*>(&tcpSocketAddr), sizeof(tcpSocketAddr));
	if (x == SOCKET_ERROR)
	{
		cout << "[TCP Port]Binding failed. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// UDP 소켓 주소 구조체 초기화
	memset(&udpSocketAddr, 0, sizeof(udpSocketAddr));

	udpSocketAddr.sin_family = AF_INET;								// Address Family Internet
	udpSocketAddr.sin_port = htons(UDP_PORT);				// 수신할 Port #
	udpSocketAddr.sin_addr.S_un.S_addr = inet_addr(IP_ADDRESS);	// 타켓 IP

	// UDP 소켓 생성
	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	// 소켓 생성이 되지 않았을 때 오류 메시지 출력 후 종료
	if (udpSocket == INVALID_SOCKET)
	{
		cout << "Game Socket Error " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// UDP 소켓 IP/Port 바인드
	int g = bind(udpSocket, (sockaddr*)&udpSocketAddr, sizeof(udpSocketAddr));
	if (g == SOCKET_ERROR)
	{
		cout << "[Game Port]Binding failed. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	// Map 및 Message 데이터 파일 읽기
	mapData = MapData::readMapDataFromFile();
	messageData = Message::readDataFromFile();

	// TCP Listen  시작
	listen(tcpListenSocket, 5);
	std::thread* tcpThread = nullptr;   // TCP 통신 전용 스레드 포인터
	bool nextWaiting = true;			// 클라이언트 대기 메시지 출력 여부

	while (true)
	{
		if(nextWaiting)
			cout << "Waiting for client..." << endl;

		// TCP 클라이언트 Accept
		int xx = sizeof(clientAddr);
		SOCKET clientSocket = accept(tcpListenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &xx);

		// 소켓에 오류가 있는지 확인
		if (clientSocket == INVALID_SOCKET)
		{
			// 수신된 데이터가 없을 경우 (WOULD_BLOCK) 다시 accept 반복
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				nextWaiting = false;
				continue;
			}

			// 그외의 경우, 오류 출력 후 반복 종료
			cout << "[Listen InvalidSocket] " << WSAGetLastError() << endl;
			break;
		}

		// 새로운 소켓 생성에 대한 메시지 출력
		cout << "Connection established. New socket num is " << clientSocket << endl;

		// 일단은 처음 접속만 새로운 방 개설로 처리함.
		if (connectedClientCount == 0)
		{
			clientSocketQueue.push(clientSocket);
			if(tcpThread == nullptr)
				tcpThread = new std::thread(messageThreadTCP);
		}
		else
		{
			// 소켓 추가에 대한 mutex lock
			MutexLockHelper lock(&socketMutex);
			clientSocketQueue.push(clientSocket);
			// 블록을 벗어나면서 mutex unlock
		}
		// 동시 접속량 갱신
		connectedClientCount++;
		cout << "[Connect] Connected Clients: " << connectedClientCount << endl;
		nextWaiting = true;
	}

	// thread 제거
	delete tcpThread;
	// 소켓 닫기
	closesocket(tcpListenSocket);
	closesocket(udpSocket);
	WSACleanup();

	return 0;
}