#include "TCPServer.h"

std::queue<SOCKET> clientSocketQueue;
std::mutex socketMutex;

/// <summary>
/// UDP���� ���� �� ����� �޽���.
/// first: PlayerName
/// second: message
/// </summary>
std::queue<std::pair<std::string, std::string>> toTcpMessageQueue;
std::mutex tcpMessageMutex;


std::string CreateTcpMessage(std::string playerName, std::string message, int& bufSize)
{
	// TCP �޽��� ����
	rapidjson::Document doc;
	doc.SetObject();
	doc.AddMember("playerName", playerName, doc.GetAllocator());
	doc.AddMember("message", message, doc.GetAllocator());
	std::string json = GetJsonString(doc, bufSize);
	return json;
}

void AccessComplete(SOCKET& client, std::unordered_map<std::string, SOCKET>& clientSockets)
{
	std::string message = messageData->GetMessageContent(Message::AccessAccept);
	int bufSize = 0;
	std::string msg = CreateTcpMessage("HANDSHAKING", message, bufSize);
	int s = send(client, msg.c_str(), bufSize, 0);
	cout << "[TCP]Access completed message sended." << "(" << s << ")" << endl;
	
	char name[BUF_SIZE];
	int n = recv(client, name, BUF_SIZE, 0);
	
	if (n == SOCKET_ERROR)
	{
		cout << "[TCP]TCP Socket Error " << WSAGetLastError() << endl;
		return;
	}

	if (n < BUF_SIZE)
		name[n] = 0;

	std::vector<std::string> jsons = split(std::string(name), '|');
	rapidjson::Document nameDoc;
	nameDoc.Parse(jsons[jsons.size() - 1].c_str());
	std::string playerName = nameDoc["playerName"].GetString();
	clientSockets.insert(make_pair(playerName, client));
}

void MessageThreadTCP()
{
	DWORD dwResult = 0;
	std::unordered_map<std::string, SOCKET> clientSockets;
	char buf[BUF_SIZE];
	//std::vector<SOCKET> clientSockets;
	SOCKET firstSocket = clientSocketQueue.front();
	clientSocketQueue.pop();

	int n = 0;
	bool connected = true;
	int clientLength = sizeof(firstSocket);

	//DWORD dwThreadID;
	//HANDLE hThread = CreateThread(NULL, 0, DataThreadUDP, nullptr, 0, &dwThreadID);
	std::thread udpThread(DataThreadUDP);

	AccessComplete(firstSocket, clientSockets);

	while (connected)
	{		
		// �ű� ���� �÷��̾� Ȯ��
		while (!clientSocketQueue.empty())
		{
			// Ŭ���̾�Ʈ ���� ť Ȯ�� (mutex lock)
			socketMutex.lock();
			SOCKET client = clientSocketQueue.front();
			clientSocketQueue.pop();
			socketMutex.unlock();

			AccessComplete(client, clientSockets);
		}

		// ��� �޽��� ť Ȯ��
		while (!toTcpMessageQueue.empty())
		{
			// TCP �޽��� ť Ȯ��
			tcpMessageMutex.lock();
			std::string playerName = toTcpMessageQueue.front().first;
			std::string message = toTcpMessageQueue.front().second;
			toTcpMessageQueue.pop();
			tcpMessageMutex.unlock();

			// playerName���� ���� �ؽ� �� ã��
			std::unordered_map<std::string, SOCKET>::iterator socketIter = clientSockets.find(playerName);
			SOCKET sock = socketIter->second;
			if (socketIter != clientSockets.end())
			{
				int bufSize = 0;
				std::string json = CreateTcpMessage(playerName, message, bufSize).c_str();
				send(sock, json.c_str(), bufSize, 0);
			}
		}

		std::unordered_map<std::string, SOCKET>::iterator iter;
		for (iter = clientSockets.begin(); iter != clientSockets.end(); ++iter)
		{

			rapidjson::Document newDocument;
			SOCKET client = iter->second;
			n = recv(client, buf, BUF_SIZE, 0); // TODO : nonblocking ����
			if (n == SOCKET_ERROR)
			{
				if (::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					cout << "[TCP][Receive Error] " << WSAGetLastError() << endl;
					break;
				}
				else
					continue;
			}
			else if (n == 0)
			{
				cout << "[TCP]Close Requested" << endl;
				clientSockets.erase(iter->first);
				connectedClientCount--;
				cout << "[TCP][Disconnect] Connected Clients: " << connectedClientCount << endl;
				if (connectedClientCount == 0)
				{
					std::string closeMsg = messageData->GetMessageContent(Message::Close);
					udpMessageMutex.lock();
					toUdpMessageHash.insert(std::make_pair("SYSTEM", closeMsg));
					udpMessageMutex.unlock();

					break;
				}

				continue;
			}

			if (n < BUF_SIZE)
				buf[n] = 0;
			else
			{
				cout << "[TCP]Too long message" << endl;
				continue;
			}

			std::vector<std::string> jsons = split(std::string(buf), '|');

			newDocument.Parse(jsons[jsons.size() - 1].c_str());

			std::string message = std::string(newDocument["message"].GetString());
			std::string playerName = std::string(newDocument["playerName"].GetString());
			cout << "[Message] " << message << endl;

			if (message.compare(messageData->GetMessageContent(Message::Close)) == 0)
			{
				//UDP���� �����ؾߵǸ� UDP ������ť�� ���?
				continue;
			}
			if (message.compare(messageData->GetMessageContent(Message::RespawnRequest)) == 0)
			{
				// ������ ó��
				std::string respawnMsg = std::string(messageData->GetMessageContent(Message::RespawnAccept));
				newDocument["message"].SetString(respawnMsg.data(), respawnMsg.size(), newDocument.GetAllocator());
				// UDP�� ������ ��û (mutex lock)
				MutexLockHelper lock(&udpMessageMutex);
				toUdpMessageHash.insert(
					std::make_pair(
						playerName, 
						messageData->GetMessageContent(Message::RespawnRequest)
					)
				);
				// ����� �����鼭 mutex�� unlock
			}

			int bufSize;
			std::string assembledJson = GetJsonString(newDocument, bufSize).c_str();
			send(client, assembledJson.c_str(), bufSize, 0);
			//cout << "[buf] " << buf << endl;
		}

		if (connectedClientCount == 0)
			break;

		cout << "[TCP]Message Checked" << endl;
		Sleep(100);
	}

	cout << "[TCP Disconnect] All clients disconnected." << endl;

	udpThread.join();
}