#include "Tools.h"

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

std::vector<std::string> split(std::string str, char Delimiter) {
	std::istringstream iss(str);             // istringstream�� str�� ��´�.
	std::string buffer;                      // �����ڸ� �������� ����� ���ڿ��� ������� ����

	std::vector<std::string> result;

	// istringstream�� istream�� ��ӹ����Ƿ� getline�� ����� �� �ִ�.
	while (getline(iss, buffer, Delimiter)) {
		result.push_back(buffer);               // ����� ���ڿ��� vector�� ����
	}

	return result;
}

const char* GetJsonString(rapidjson::Document& doc, int& size)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string msg = buffer.GetString();
	msg.append("|");
	const char* jsonString = msg.c_str();
	size = buffer.GetSize();

	return jsonString;
}
