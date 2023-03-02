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

#pragma comment(lib, "ws2_32.lib")

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#define SERVER_PORT 9999
#define BUF_SIZE 4096
#define QUEUE_SIZE 10
#define IP_ADDRESS "127.0.0.1"

std::queue<SOCKET> ClientSocketQueue;

DWORD ThreadID[5];
int count = 0;

int getCurrentTimeInMilliSeconds();

DWORD WINAPI PlayServer(LPVOID);
std::vector<std::string> split(std::string str, char Delimiter);

// TODO : 이런 규칙 데이터는 json 같이 따로 관리하기 (임시방편)
const int limitY = -10;