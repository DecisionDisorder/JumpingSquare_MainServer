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

#include "MutexLock.h"
#include "ServerSettings.h"
#include "PlayerData.h"
#include "TCPServer.h"

extern SOCKET udpSocket;

extern std::unordered_map<std::string, std::string> toUdpMessageHash;
extern std::mutex udpMessageMutex;


bool CheckClose();
void DataThreadUDP();