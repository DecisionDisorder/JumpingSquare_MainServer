#pragma once

#include "Vector3.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include <vector>

class Map
{
private:
	std::string name;
	int index;
	Vector3 startPosition;
	Vector3 startRotation;
	Vector3 clearPosition;
	Vector3 clearBoundary;

public:
	Map(rapidjson::Value& doc);

	inline Vector3 GetStartPosition() { return startPosition; }
	inline Vector3 GetStartRotation() { return startRotation; }
	inline Vector3 GetClearPosition() { return clearPosition; }
	inline Vector3 GetClearBoundary() { return clearBoundary; }
};

class MapData
{
private:
	float limitY;
	std::vector<Map> maps;

public:
	MapData(rapidjson::Document &doc);

	inline float GetLimitY() { return limitY; }
	inline Map GetMap(int index) { return maps[index]; }
};