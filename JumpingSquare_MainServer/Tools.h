/*
 * 도구 모음 헤더 
 */

#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include "rapidjson/document.h"

namespace dedi
{
	struct Vector3;
}

namespace dedi
{
	/// <summary>
	/// 특정 문자 기준으로 문자열 나누기
	/// </summary>
	/// <param name="str">나눌 문자열</param>
	/// <param name="Delimiter">기준 문자</param>
	/// <returns>쪼개진 문자열 리스트</returns>
	std::vector<std::string> split(std::string str, const char delimiter);

	/// <summary>
	/// 현재 시간을 ms단위의 Unix epoch time 값으로 가져온다.
	/// </summary>

	int getCurrentTimeInMilliSeconds();
	/// <summary>
	/// 1차원 형태로 범위 계산
	/// </summary>
	/// <param name="playerPosition">플레이어 위치</param>
	/// <param name="targetPosition">기준점 위치</param>
	/// <param name="targetBoundary">기준점 범위</param>
	/// <returns>범위 내에 들어왔는지 여부</returns>

	bool checkBoundary1D(const float playerPosition, const float targetPosition, const float targetBoundary);
	/// <summary>
	/// 3차원 형태로 범위 계산
	/// </summary>
	/// <param name="playerPosition">플레이어 위치</param>
	/// <param name="targetPosition">기준점 위치</param>
	/// <param name="targetBoundary">기준점 범위</param>
	/// <returns>범위 내에 들어왔는지 여부</returns>
	bool checkBoundary3D(const Vector3& playerPosition, const Vector3& targetPosition, const Vector3& targetBoundary);

	/// <summary>
	/// Json Document를 Json String으로 변환
	/// </summary>
	/// <param name="doc">Json Document</param>
	/// <param name="size">버퍼 크기</param>
	/// <returns>Json String</returns>
	std::string getJsonString(rapidjson::Document& doc, int& size);
}