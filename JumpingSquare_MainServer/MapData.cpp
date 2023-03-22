#include "MapData.h"

Map::Map(rapidjson::Value& val)
{
	name = val["name"].GetString();
	index = val["index"].GetInt();
	startPosition = Vector3(val["startPosition"]["x"].GetDouble(), val["startPosition"]["y"].GetDouble(), val["startPosition"]["z"].GetDouble());
	startRotation = Vector3(val["startRotation"]["x"].GetDouble(), val["startRotation"]["y"].GetDouble(), val["startRotation"]["z"].GetDouble());
	clearPosition = Vector3(val["clearPosition"]["x"].GetDouble(), val["clearPosition"]["y"].GetDouble(), val["clearPosition"]["z"].GetDouble());
	clearBoundary = Vector3(val["clearBoundary"]["x"].GetDouble(), val["clearBoundary"]["y"].GetDouble(), val["clearBoundary"]["z"].GetDouble());

}

MapData::MapData(rapidjson::Document &doc)
{
	limitY = doc["death_zone_y"].GetDouble();
	rapidjson::Value& mapValues = doc["maps"];
	assert(mapValues.IsArray());
	for (int i = 0; i < mapValues.Size(); i++)
	{
		Map map = Map(mapValues[i]);
		maps.push_back(map);
	}
}

MapData* MapData::ReadMapDataFromFile()
{
	std::ifstream fIn("data/map_data.json");
	std::string str;
	MapData* mapData = nullptr;

	if (fIn.is_open())
	{
		rapidjson::Document doc;
		fIn >> str;
		doc.Parse(const_cast<char*>(str.c_str()));

		mapData = new MapData(doc);

		fIn.close();
	}

	return mapData;
}