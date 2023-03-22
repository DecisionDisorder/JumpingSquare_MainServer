#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
using std::cout;
using std::endl;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#include <hash_map>
#include <queue>

#include "ServerSettings.h"
#include "PlayerData.h"
#include "TCPServer.h"

SOCKET udpSocket;

std::queue<PlayerData> dataQueue;
DWORD WINAPI DataThreadUDP(LPVOID);