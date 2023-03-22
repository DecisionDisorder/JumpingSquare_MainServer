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
#include <hash_map>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "ServerSettings.h"
#include "PlayerData.h"
#include "Tools.h"
#include "Message.h"
#include "UDPServer.h"

std::queue<SOCKET> clientSocketQueue;
/// <summary>
/// UDP에서 검증 후 등록한 메시지.
/// first: PlayerName
/// second: message
/// </summary>
std::queue<std::pair<std::string, std::string>> messageQueue;

DWORD WINAPI MessageThreadTCP(LPVOID);
void AccessComplete(SOCKET client, stdext::hash_map<std::string, SOCKET>& clientSockets);

