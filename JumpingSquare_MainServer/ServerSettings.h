#pragma once

namespace dedi
{
	class MapData;
	class Message;
}

namespace dedi
{
	// TCP 수신 포트 번호
	#define TCP_PORT 49990
	// UDP 수신 포트 번호
	#define UDP_PORT 49999
	// 메시지 최대 버퍼 크기
	#define BUF_SIZE 1024
	// 서버 IP 주소
	#define IP_ADDRESS "172.30.1.49"

	// 접속중인 유저 수
	extern int connectedClientCount;
	// 맵 데이터
	extern MapData* mapData;
	// 메시지 데이터
	extern Message* messageData;
}