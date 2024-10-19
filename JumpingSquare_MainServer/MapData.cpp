#include "MapData.h"

namespace dedi
{
	Map::Map(rapidjson::Value& val)
	{
		// 개별 맵에 대한 Json Value에서 값을 불러와서 초기화
		_name = val["name"].GetString();
		_index = val["index"].GetInt();
		_startPosition = Vector3(val["startPosition"]["x"].GetDouble(), val["startPosition"]["y"].GetDouble(), val["startPosition"]["z"].GetDouble());
		_startRotation = Vector3(val["startRotation"]["x"].GetDouble(), val["startRotation"]["y"].GetDouble(), val["startRotation"]["z"].GetDouble());
		_clearPosition = Vector3(val["clearPosition"]["x"].GetDouble(), val["clearPosition"]["y"].GetDouble(), val["clearPosition"]["z"].GetDouble());
		_clearBoundary = Vector3(val["clearBoundary"]["x"].GetDouble(), val["clearBoundary"]["y"].GetDouble(), val["clearBoundary"]["z"].GetDouble());

	}

	MapData::MapData(rapidjson::Document& doc)
	{
		// Json Document에서 값을 불러와서 초기화
		limitY = doc["death_zone_y"].GetDouble();
		rapidjson::Value& mapValues = doc["maps"];
		// 개별 맵에 대한 데이터 초기화
		assert(mapValues.IsArray());
		for (unsigned int i = 0; i < mapValues.Size(); i++)
		{
			Map map = Map(mapValues[i]);
			maps.push_back(map);
		}
	}

	MapData* MapData::readMapDataFromFile()
	{
		// 맵 데이터 파일을 연다
		std::ifstream fIn("data/map_data.json");
		std::string str;
		MapData* loadedMapData = nullptr;

		// 파일을 불러와서 문자열을 읽어오고, 데이터를 초기화하여 반환
		if (fIn.is_open())
		{
			rapidjson::Document doc;
			fIn >> str;
			doc.Parse(const_cast<char*>(str.c_str()));

			loadedMapData = new MapData(doc);

			fIn.close();
		}

		return loadedMapData;
	}
}