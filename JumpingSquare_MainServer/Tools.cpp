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
	// x, y, z에 대하여 범위 계산
	if (CheckBoundary1D(playerPosition.x, targetPosition.x, targetBoundary.x) &&
		CheckBoundary1D(playerPosition.y, targetPosition.y, targetBoundary.y) &&
		CheckBoundary1D(playerPosition.z, targetPosition.z, targetBoundary.z))
		return true;

	return false;
}

std::vector<std::string> split(std::string str, char Delimiter) {
	std::istringstream iss(str);             // istringstream에 str을 담는다.
	std::string buffer;                      // 구분자를 기준으로 절삭된 문자열이 담겨지는 버퍼

	std::vector<std::string> result;		 // 결과 문자열 리스트

	// istringstream은 istream을 상속받으므로 getline을 사용할 수 있다.
	while (getline(iss, buffer, Delimiter)) {
		// 나뉜 문자열을 vector에 저장
		result.push_back(buffer); 
	}

	return result;
}

std::string GetJsonString(rapidjson::Document& doc, int& size)
{
	// Json Document -> Json String 변환
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string msg = buffer.GetString();
	// 구분자 추가
	msg.append("|");
	// 버퍼 크기 계산
	size = buffer.GetSize() + 1;

	return msg;
}
