/*
 * ���� ���� ��� 
 */

#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <sstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "Vector3.h"

/// <summary>
/// Ư�� ���� �������� ���ڿ� ������
/// </summary>
/// <param name="str">���� ���ڿ�</param>
/// <param name="Delimiter">���� ����</param>
/// <returns>�ɰ��� ���ڿ� ����Ʈ</returns>
std::vector<std::string> split(std::string str, char Delimiter);
/// <summary>
/// ���� �ð��� ms������ Unix epoch time ������ �����´�.
/// </summary>
int GetCurrentTimeInMilliSeconds();
/// <summary>
/// 1���� ���·� ���� ���
/// </summary>
/// <param name="playerPosition">�÷��̾� ��ġ</param>
/// <param name="targetPosition">������ ��ġ</param>
/// <param name="targetBoundary">������ ����</param>
/// <returns>���� ���� ���Դ��� ����</returns>
bool CheckBoundary1D(float playerPosition, float targetPosition, float targetBoundary);
/// <summary>
/// 3���� ���·� ���� ���
/// </summary>
/// <param name="playerPosition">�÷��̾� ��ġ</param>
/// <param name="targetPosition">������ ��ġ</param>
/// <param name="targetBoundary">������ ����</param>
/// <returns>���� ���� ���Դ��� ����</returns>
bool CheckBoundary3D(Vector3 playerPosition, const Vector3 targetPosition, const Vector3 targetBoundary);
/// <summary>
/// Json Document�� Json String���� ��ȯ
/// </summary>
/// <param name="doc">Json Document</param>
/// <param name="size">���� ũ��</param>
/// <returns>Json String</returns>
std::string GetJsonString(rapidjson::Document& doc, int& size);