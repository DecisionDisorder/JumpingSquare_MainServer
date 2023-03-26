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

// RapidJson���� std::string�� ��밡���ϵ��� ó��
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

// ���� ������� Ŭ���̾�Ʈ ���� ť
extern std::queue<SOCKET> clientSocketQueue;
// Ŭ���̾�Ʈ ���� ť�� ����ȭ�� ��ȣ�ϴ� Mutex
extern std::mutex socketMutex;

// TCP�� ���� ���� �޽��� ť (first: playerName, second: message)
extern std::queue<std::pair<std::string, std::string>> toTcpMessageQueue;
// �޽��� ť�� ����ȭ�� ��ȣ�ϴ� Mutex
extern std::mutex tcpMessageMutex;

/// <summary>
/// Ŭ���̾�Ʈ�� ���� TCP �޽��� ����
/// </summary>
/// <param name="playerName">�÷��̾� �̸�</param>
/// <param name="message">�޽��� ����</param>
/// <param name="bufSize">���� ũ��</param>
/// <returns>������ �޽��� ���ڿ�</returns>
std::string CreateTcpMessage(std::string playerName, std::string message, int& bufSize);
/// <summary>
/// TCP ���� �޽��� �ۼ��� ó�� ������
/// </summary>
void MessageThreadTCP();
/// <summary>
/// ���� �Ϸ� ó�� �Լ�
/// </summary>
/// <param name="client">���� ��� Ŭ���̾�Ʈ ����</param>
/// <param name="clientSockets">������ Ŭ���̾�Ʈ �ؽ�</param>
void AccessComplete(SOCKET& client, std::unordered_map<std::string, SOCKET>& clientSockets);

