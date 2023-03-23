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

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "MutexLock.h"
#include "ServerSettings.h"
#include "PlayerData.h"
#include "Tools.h"
#include "Message.h"
#include "UDPServer.h"

extern std::queue<SOCKET> clientSocketQueue;
extern std::mutex socketMutex;

extern std::queue<std::pair<std::string, std::string>> toTcpMessageQueue;
extern std::mutex tcpMessageMutex;

std::string CreateTcpMessage(std::string playerName, std::string message, int& bufSize);
void MessageThreadTCP();
void AccessComplete(SOCKET& client, std::unordered_map<std::string, SOCKET>& clientSockets);

