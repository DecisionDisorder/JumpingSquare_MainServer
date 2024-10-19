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

#include <vector>
#include <queue>
#include <string>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")
