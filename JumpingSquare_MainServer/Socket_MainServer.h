#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
using std::cout;
using std::endl;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#include <random>
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

#include <thread>
#include <chrono>

#include <vector>
#include <sstream>
#include <queue>
#include <string>
#include <stdexcept>

#include <fstream>

#pragma comment(lib, "ws2_32.lib")

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "Vector3.h"
#include "PlayerData.h"
#include "MapData.h"

#define SERVER_PORT 48080
#define BUF_SIZE 4096
#define QUEUE_SIZE 10
#define IP_ADDRESS "172.30.1.49"

std::queue<SOCKET> ClientSocketQueue;

DWORD ThreadID[5];
int count = 0;

void ReadMapData();
int GetCurrentTimeInMilliSeconds();
bool CheckBoundary1D(float playerPosition, float targetPosition, float targetBoundary);
bool CheckBoundary3D(Vector3 playerPosition, const Vector3 targetPosition, const Vector3 targetBoundary);

DWORD WINAPI PlayServer(LPVOID);
std::vector<std::string> split(std::string str, char Delimiter);

MapData *mapData;