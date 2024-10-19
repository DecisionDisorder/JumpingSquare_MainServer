#pragma once

namespace dedi
{
	class MapData;
	class Message;
}

namespace dedi
{
	// TCP ���� ��Ʈ ��ȣ
	#define TCP_PORT 49990
	// UDP ���� ��Ʈ ��ȣ
	#define UDP_PORT 49999
	// �޽��� �ִ� ���� ũ��
	#define BUF_SIZE 1024
	// ���� IP �ּ�
	#define IP_ADDRESS "172.30.1.49"

	// �������� ���� ��
	extern int connectedClientCount;
	// �� ������
	extern MapData* mapData;
	// �޽��� ������
	extern Message* messageData;
}