#include "MapData.h"

namespace dedi
{
	Map::Map(rapidjson::Value& val)
	{
		// ���� �ʿ� ���� Json Value���� ���� �ҷ��ͼ� �ʱ�ȭ
		_name = val["name"].GetString();
		_index = val["index"].GetInt();
		_startPosition = Vector3(val["startPosition"]["x"].GetDouble(), val["startPosition"]["y"].GetDouble(), val["startPosition"]["z"].GetDouble());
		_startRotation = Vector3(val["startRotation"]["x"].GetDouble(), val["startRotation"]["y"].GetDouble(), val["startRotation"]["z"].GetDouble());
		_clearPosition = Vector3(val["clearPosition"]["x"].GetDouble(), val["clearPosition"]["y"].GetDouble(), val["clearPosition"]["z"].GetDouble());
		_clearBoundary = Vector3(val["clearBoundary"]["x"].GetDouble(), val["clearBoundary"]["y"].GetDouble(), val["clearBoundary"]["z"].GetDouble());

	}

	MapData::MapData(rapidjson::Document& doc)
	{
		// Json Document���� ���� �ҷ��ͼ� �ʱ�ȭ
		limitY = doc["death_zone_y"].GetDouble();
		rapidjson::Value& mapValues = doc["maps"];
		// ���� �ʿ� ���� ������ �ʱ�ȭ
		assert(mapValues.IsArray());
		for (unsigned int i = 0; i < mapValues.Size(); i++)
		{
			Map map = Map(mapValues[i]);
			maps.push_back(map);
		}
	}

	MapData* MapData::readMapDataFromFile()
	{
		// �� ������ ������ ����
		std::ifstream fIn("data/map_data.json");
		std::string str;
		MapData* loadedMapData = nullptr;

		// ������ �ҷ��ͼ� ���ڿ��� �о����, �����͸� �ʱ�ȭ�Ͽ� ��ȯ
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