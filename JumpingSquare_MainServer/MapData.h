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
	/// 개별 맵에 대한 데이터 클래스
	/// </summary>
	class Map
	{
	public:
		Map(rapidjson::Value& doc);

		/// <summary>
		/// 시작 위치를 불러온다
		/// </summary>
		inline Vector3 getStartPosition() { return _startPosition; }
		/// <summary>
		/// 시작 회전 값을 불러온다
		/// </summary>
		inline Vector3 getStartRotation() { return _startRotation; }
		/// <summary>
		/// 클리어 기준 위치를 불러온다
		/// </summary>
		inline Vector3 getClearPosition() { return _clearPosition; }
		/// <summary>
		/// 클리어 판정 범위를 불러온다.
		/// </summary>
		inline Vector3 getClearBoundary() { return _clearBoundary; }

	private:
		// 맵 이름
		std::string _name;
		// 맵 인덱스
		int _index;
		// 플레이 시작 위치 좌표
		Vector3 _startPosition;
		// 플레이 시작 회전 값
		Vector3 _startRotation;
		// 클리어 기준 위치
		Vector3 _clearPosition;
		// 클리어 판정 범위
		Vector3 _clearBoundary;
	};

	/// <summary>
	/// 전체 맵 데이터 관리 클래스
	/// </summary>
	class MapData
	{
	private:
		// 사망 처리 기준 y좌표
		float limitY;
		// 맵 데이터 리스트
		std::vector<Map> maps;

	public:
		MapData(rapidjson::Document& doc);

		/// <summary>
		/// 사망 처리 기준 y좌표를 읽어온다.
		/// </summary>
		/// <returns>사망 기준 Y</returns>
		inline float getLimitY() { return limitY; }
		/// <summary>
		/// 개별 맵 데이터를 불러온다.
		/// </summary>
		/// <param name="index">맵 인덱스</param>
		/// <returns>맵 데이터</returns>
		inline Map getMap(int index) { return maps[index]; }

		/// <summary>
		/// 맵 데이터를 파일로부터 읽어서 반환한다
		/// </summary>
		/// <returns>맵 데이터</returns>
		static MapData* readMapDataFromFile();
	};
}