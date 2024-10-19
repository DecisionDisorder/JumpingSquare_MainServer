#pragma once

#include "Vector3.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include <vector>
#include <fstream>

namespace dedi
{
	/// <summary>
	/// ���� �ʿ� ���� ������ Ŭ����
	/// </summary>
	class Map
	{
	public:
		Map(rapidjson::Value& doc);

		/// <summary>
		/// ���� ��ġ�� �ҷ��´�
		/// </summary>
		inline Vector3 getStartPosition() { return _startPosition; }
		/// <summary>
		/// ���� ȸ�� ���� �ҷ��´�
		/// </summary>
		inline Vector3 getStartRotation() { return _startRotation; }
		/// <summary>
		/// Ŭ���� ���� ��ġ�� �ҷ��´�
		/// </summary>
		inline Vector3 getClearPosition() { return _clearPosition; }
		/// <summary>
		/// Ŭ���� ���� ������ �ҷ��´�.
		/// </summary>
		inline Vector3 getClearBoundary() { return _clearBoundary; }

	private:
		// �� �̸�
		std::string _name;
		// �� �ε���
		int _index;
		// �÷��� ���� ��ġ ��ǥ
		Vector3 _startPosition;
		// �÷��� ���� ȸ�� ��
		Vector3 _startRotation;
		// Ŭ���� ���� ��ġ
		Vector3 _clearPosition;
		// Ŭ���� ���� ����
		Vector3 _clearBoundary;
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
		MapData(rapidjson::Document& doc);

		/// <summary>
		/// ��� ó�� ���� y��ǥ�� �о�´�.
		/// </summary>
		/// <returns>��� ���� Y</returns>
		inline float getLimitY() { return limitY; }
		/// <summary>
		/// ���� �� �����͸� �ҷ��´�.
		/// </summary>
		/// <param name="index">�� �ε���</param>
		/// <returns>�� ������</returns>
		inline Map getMap(int index) { return maps[index]; }

		/// <summary>
		/// �� �����͸� ���Ϸκ��� �о ��ȯ�Ѵ�
		/// </summary>
		/// <returns>�� ������</returns>
		static MapData* readMapDataFromFile();
	};
}