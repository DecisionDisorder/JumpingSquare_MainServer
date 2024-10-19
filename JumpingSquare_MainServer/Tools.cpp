#include "Tools.h"
#include "Vector3.h"
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace dedi
{
	int getCurrentTimeInMilliSeconds()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	bool checkBoundary1D(const float playerPosition, const float targetPosition, const float targetBoundary)
	{
		if (targetPosition - targetBoundary <= playerPosition && playerPosition <= targetPosition + targetBoundary)
			return true;

		return false;
	}


	bool checkBoundary3D(const Vector3& playerPosition, const Vector3& targetPosition, const Vector3& targetBoundary)
	{
		// x, y, z�� ���Ͽ� ���� ���
		if (checkBoundary1D(playerPosition.x, targetPosition.x, targetBoundary.x) &&
			checkBoundary1D(playerPosition.y, targetPosition.y, targetBoundary.y) &&
			checkBoundary1D(playerPosition.z, targetPosition.z, targetBoundary.z))
			return true;

		return false;
	}

	std::vector<std::string> split(std::string str, const char delimiter) {
		std::istringstream iss(str);             // istringstream�� str�� ��´�.
		std::string buffer;                      // �����ڸ� �������� ����� ���ڿ��� ������� ����
		std::vector<std::string> result;		 // ��� ���ڿ� ����Ʈ

		// istringstream�� istream�� ��ӹ����Ƿ� getline�� ����� �� �ִ�.
		while (getline(iss, buffer, delimiter)) {
			// ���� ���ڿ��� vector�� ����
			result.push_back(buffer); 
		}

		return result;
	}

	std::string getJsonString(rapidjson::Document& doc, int& size)
	{
		// Json Document -> Json String ��ȯ
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);
		std::string msg = buffer.GetString();
		// ������ �߰�
		msg.append("|");
		// ���� ũ�� ���
		const size_t bufferSize = buffer.GetSize() + 1;
		if (bufferSize > INT_MAX)
		{
			std::cout << "[JSON][Buffer Size Error: Exceeded BufferSize(" << bufferSize << ")]" << std::endl;
			return std::string();
		}

		size = static_cast<int>(bufferSize);
		return msg;
	}
}
