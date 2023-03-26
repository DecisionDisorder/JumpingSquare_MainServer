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

// UDP ���� ����
extern SOCKET udpSocket;
// UDP�� ���� ���� �޽��� �ؽ� (first: playerName, second: message)
extern std::unordered_map<std::string, std::string> toUdpMessageHash;
// �޽��� �ؽ��� ����ȭ�� ��ȣ�ϴ� Mutex
extern std::mutex udpMessageMutex;

/// <summary>
/// UDP ������ �����ؾ��ϴ��� Ȯ���ϴ� �Լ�
/// </summary>
bool CheckClose();

/// <summary>
/// Ŭ���̾�Ʈ ���� ����Ʈ�� �ߺ��� ������ �ִ��� Ȯ��
/// </summary>
/// <param name="clientSockets">���� ���� Ŭ���̾�Ʈ ����Ʈ</param>
/// <param name="newClient">�߰��� Ŭ���̾�Ʈ</param>
/// <returns>�ߺ� ����</returns>
bool CheckDuplicateClient(std::vector<SOCKADDR_IN>& clientSockets, SOCKADDR_IN newClient);

/// <summary>
/// �÷��̾� ������ ��������� Json Document�� �ݿ�
/// </summary>
/// <param name="doc">�ݿ��� Document</param>
/// <param name="player">�ݿ��� ������</param>
void ApplyPlayerPositionToDatagram(rapidjson::Document& doc, PlayerData player);

/// <summary>
/// UDP �����͸� �ۼ����ϴ� ������
/// </summary>
void DataThreadUDP();