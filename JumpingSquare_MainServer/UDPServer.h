#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
using std::cout;
using std::endl;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#include <unordered_map>
#include <queue>
#include <mutex>

#include "rapidjson/document.h"

namespace dedi
{
	class PlayerData;
}

namespace dedi
{
	// UDP 수신 소켓
	extern SOCKET udpSocket;
	// UDP로 보낸 내부 메시지 해시 (first: playerName, second: message)
	extern std::unordered_map<std::string, std::string> toUdpMessageHash;
	// 메시지 해시의 동기화를 보호하는 Mutex
	extern std::mutex udpMessageMutex;

	/// <summary>
	/// UDP 서버를 종료해야하는지 확인하는 함수
	/// </summary>
	bool checkClose();

	/// <summary>
	/// 클라이언트 소켓 리스트에 중복된 소켓이 있는지 확인
	/// </summary>
	/// <param name="clientSockets">접속 중인 클라이언트 리스트</param>
	/// <param name="newClient">추가할 클라이언트</param>
	/// <returns>중복 여부</returns>
	bool checkDuplicateClient(std::vector<SOCKADDR_IN>& clientSockets, SOCKADDR_IN newClient);

	/// <summary>
	/// 플레이어 데이터 변경사항을 Json Document에 반영
	/// </summary>
	/// <param name="doc">반영될 Document</param>
	/// <param name="player">반영할 데이터</param>
	void applyPlayerPositionToDatagram(rapidjson::Document& doc, const PlayerData& player);

	/// <summary>
	/// UDP 데이터를 송수신하는 스레드
	/// </summary>
	void dataThreadUDP();
}