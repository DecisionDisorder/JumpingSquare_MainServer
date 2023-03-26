#pragma once

#include "Vector3.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include <vector>
#include <fstream>

/// <summary>
/// ���� �ʿ� ���� ������ Ŭ����
/// </summary>
class Map
{
private:
	// �� �̸�
	std::string name;
	// �� �ε���
	int index;
	// �÷��� ���� ��ġ ��ǥ
	Vector3 startPosition;
	// �÷��� ���� ȸ�� ��
	Vector3 startRotation;
	// Ŭ���� ���� ��ġ
	Vector3 clearPosition;
	// Ŭ���� ���� ����
	Vector3 clearBoundary;

public:
	Map(rapidjson::Value& doc);

	/// <summary>
	/// ���� ��ġ�� �ҷ��´�
	/// </summary>
	inline Vector3 GetStartPosition() { return startPosition; }
	/// <summary>
	/// ���� ȸ�� ���� �ҷ��´�
	/// </summary>
	inline Vector3 GetStartRotation() { return startRotation; }
	/// <summary>
	/// Ŭ���� ���� ��ġ�� �ҷ��´�
	/// </summary>
	inline Vector3 GetClearPosition() { return clearPosition; }
	/// <summary>
	/// Ŭ���� ���� ������ �ҷ��´�.
	/// </summary>
	inline Vector3 GetClearBoundary() { return clearBoundary; }
};

/// <summary>
/// ��ü �� ������ ���� Ŭ����
/// </summary>
class MapData
{
private:
	// ��� ó�� ���� y��ǥ
	float limitY;
	// �� ������ ����Ʈ
	std::vector<Map> maps;

public:
	MapData(rapidjson::Document &doc);

	/// <summary>
	/// ��� ó�� ���� y��ǥ�� �о�´�.
	/// </summary>
	/// <returns>��� ���� Y</returns>
	inline float GetLimitY() { return limitY; }
	/// <summary>
	/// ���� �� �����͸� �ҷ��´�.
	/// </summary>
	/// <param name="index">�� �ε���</param>
	/// <returns>�� ������</returns>
	inline Map GetMap(int index) { return maps[index]; }

	/// <summary>
	/// �� �����͸� ���Ϸκ��� �о ��ȯ�Ѵ�
	/// </summary>
	/// <returns>�� ������</returns>
	static MapData* ReadMapDataFromFile();
};