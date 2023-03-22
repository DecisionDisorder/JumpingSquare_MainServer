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
	std::istringstream iss(str);             // istringstream에 str을 담는다.
	std::string buffer;                      // 구분자를 기준으로 절삭된 문자열이 담겨지는 버퍼

	std::vector<std::string> result;

	// istringstream은 istream을 상속받으므로 getline을 사용할 수 있다.
	while (getline(iss, buffer, Delimiter)) {
		result.push_back(buffer);               // 절삭된 문자열을 vector에 저장
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
