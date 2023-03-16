#include "Socket_MainServer.h"


int main()
{
	WORD		wVersionRequested;
	WSADATA		wsaData;
	SOCKADDR_IN serverAddr, clientAddr;
	int			err;
	int			byteSent;

	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	
	if (err != 0) {
		cout << "WSAStartup error " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	serverAddr.sin_family = AF_INET; // Address Family Internet
	serverAddr.sin_port = htons(SERVER_PORT); // Port to connect on
	serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS); // Target IP

	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // TCP 소켓 생성
	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "Socket Error " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	int x = bind(ListenSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr));
	if (x == SOCKET_ERROR)
	{
		cout << "Binding failed. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	listen(ListenSocket, 5);

	ReadMapData();

	while (true)
	{
		cout << "Waiting for client..." << endl;
		int xx = sizeof(clientAddr);
		SOCKET ClientSocket = accept(ListenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &xx);
		cout << "Connection established. New socket num is " << ClientSocket << endl;

		// 일단은 처음 접속만 새로운 방 개설로 처리함.
if (count == 0)
{
	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, PlayServer, (LPVOID)ClientSocket, 0, &dwThreadID);
}
else
{
	ClientSocketQueue.push(ClientSocket);
}
count++;
cout << "[Connect] Connected Clients: " << count << endl;
	}

	closesocket(ListenSocket);
	WSACleanup();

	return 0;
}

void ReadMapData()
{
	std::ifstream fIn("data/map_data.json");
	std::string str;
	if (fIn.is_open()) 
	{
		rapidjson::Document doc;
		fIn >> str;
		doc.Parse(const_cast<char*>(str.c_str()));

		mapData = new MapData(doc);

		fIn.close();
	}
}

int GetCurrentTimeInMilliSeconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

bool CheckBoundary1D(float playerPosition, float targetPosition, float targetBoundary)
{
	if (targetPosition - targetBoundary <= playerPosition && playerPosition <= targetPosition + targetBoundary)
		return true;
	
	return false;
}


bool CheckBoundary3D(Vector3 playerPosition, const Vector3 targetPosition, const Vector3 targetBoundary)
{
	if (CheckBoundary1D(playerPosition.x, targetPosition.x, targetBoundary.x) &&
		CheckBoundary1D(playerPosition.y, targetPosition.y, targetBoundary.y) &&
		CheckBoundary1D(playerPosition.z, targetPosition.z, targetBoundary.z))
		return true;
	
	return false;
}

DWORD WINAPI PlayServer(LPVOID pvParam)
{
	DWORD dwResult = 0;
	char buf[BUF_SIZE];
	std::vector<SOCKET> ClientSockets;
	ClientSockets.push_back((SOCKET)pvParam);

	int n = 0;
	bool connected = true;
	std::vector<PlayerData> playerDataList;

	while (connected)
	{
		playerDataList.clear();
		cout << endl;
		while (!ClientSocketQueue.empty())
		{
			ClientSockets.push_back(ClientSocketQueue.front());
			ClientSocketQueue.pop();
		}

		rapidjson::Document arrDoc;
		rapidjson::Value arrayVal;
		rapidjson::Document newDocument;
		rapidjson::Document::AllocatorType& allocator = arrDoc.GetAllocator();

		arrayVal.SetArray();
		arrDoc.SetObject();

		for (int i = 0; i < ClientSockets.size(); i++)
		{
			n = recv(ClientSockets[i], buf, 4096, 0);
			if (n <= 0) {
				cout << "Got nothing" << endl;
				connected = false;
				break;
			}
			if (n < 4096)
				buf[n] = 0;
			else
			{
				cout << "Too long message" << endl;
				continue;
			}

			std::vector<std::string> jsons = split(std::string(buf), '|');

			// 받은 buf를 Json Array에  추가
			newDocument.Parse(jsons[jsons.size() - 1].c_str());

			PlayerData player(newDocument);
			playerDataList.push_back(player);

			std::string message = std::string(newDocument["message"].GetString());
			cout << "[Message] " << message << endl;

			if (player.GetPosition().y < mapData->GetLimitY() && player.IsAlive())
			{
				// TODO : 메시지 모음 정리
				newDocument["message"] = "Death";
			}
			if (message.compare("RequestRespawn") == 0)
			{
				// 리스폰 처리
				newDocument["message"] = "RespawnAccepted";
				Vector3 respawnPos = player.Respawn();
				newDocument["positionX"] = respawnPos.x;
				newDocument["positionY"] = respawnPos.y;
				newDocument["positionZ"] = respawnPos.z;
			}

			int map = newDocument["map"].GetInt();
			Vector3 clearPosition = mapData->GetMap(map).GetClearPosition();
			Vector3 clearBoundary = mapData->GetMap(map).GetClearBoundary();
			if (CheckBoundary3D(player.GetPosition(), clearPosition, clearBoundary))
			{
				// 클리어 처리
				newDocument["message"] = "StageClear";
			}

			arrayVal.PushBack(newDocument, allocator);
			//cout << "[buf] " << buf << endl;
		}

		arrDoc.AddMember("Items", arrayVal, allocator);
		cout << "Position Received" << endl;
		
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		arrDoc.Accept(writer);

		std::string msg = buffer.GetString();
		msg.append("|");

		const char* assembledJson = msg.c_str();

		cout << "Assembled Data: " << assembledJson << endl;

		// TODO: Json Array를 모든 client에 Send
		for (int i = 0; i < ClientSockets.size(); i++)
			send(ClientSockets[i], assembledJson, buffer.GetSize(), 0);
		cout << "Position Broadcasted" << endl;
	}
	
	count--;
	cout << "[Disconnect] Connected Clients: " << count << endl;

	return (dwResult);
}

std::vector<std::string> split(std::string str, char Delimiter) {
	std::istringstream iss(str);             // istringstream에 str을 담는다.
	std::string buffer;                      // 구분자를 기준으로 절삭된 문자열이 담겨지는 버퍼

	std::vector<std::string> result;

	// istringstream은 istream을 상속받으므로 getline을 사용할 수 있다.
	while (getline(iss, buffer, Delimiter)) {
		result.push_back(buffer);               // 절삭된 문자열을 vector에 저장
	}

	return result;
}