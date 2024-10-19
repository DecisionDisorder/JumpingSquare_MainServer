#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
using std::cout;
using std::endl;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#include <queue>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>

// RapidJson에서 std::string이 사용가능하도록 처리
#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace dedi
{
	// 접속 대기중인 클라이언트 소켓 큐
	extern std::queue<SOCKET> clientSocketQueue;
	// 클라이언트 소켓 큐의 동기화를 보호하는 Mutex
	extern std::mutex socketMutex;

	// TCP로 보낸 내부 메시지 큐 (first: playerName, second: message)
	extern std::queue<std::pair<std::string, std::string>> toTcpMessageQueue;
	// 메시지 큐의 동기화를 보호하는 Mutex
	extern std::mutex tcpMessageMutex;

	/// <summary>
	/// 클라이언트로 보낼 TCP 메시지 생성
	/// </summary>
	/// <param name="playerName">플레이어 이름</param>
	/// <param name="message">메시지 내용</param>
	/// <param name="bufSize">버퍼 크기</param>
	/// <returns>생성된 메시지 문자열</returns>
	std::string createTcpMessage(std::string playerName, std::string message, int& bufSize);
	/// <summary>
	/// 접속 완료 처리 함수
	/// </summary>
	/// <param name="client">접속 대상 클라이언트 소켓</param>
	/// <param name="clientSockets">접속한 클라이언트 해시</param>
	void accessComplete(SOCKET& client, std::unordered_map<std::string, SOCKET>& clientSockets);
	/// <summary>
	/// TCP 전용 메시지 송수신 처리 스레드
	/// </summary>
	void messageThreadTCP();
}