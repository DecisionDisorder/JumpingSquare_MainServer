#include "PlayerData.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

PlayerData::PlayerData(rapidjson::Document& document)
{
	name = std::string(document["playerName"].GetString());
	position = Vector3(document["positionX"].GetDouble(), document["positionY"].GetDouble(), document["positionZ"].GetDouble());
	rotation = Vector3(document["rotationX"].GetDouble(), document["rotationY"].GetDouble(), document["rotationZ"].GetDouble());
	alive = document["alive"].GetBool();
	respawnPosition = Vector3(0.f, 1.0f, 0.f);

}

void PlayerData::SetPosition(Vector3 position)
{
	this->position = position;
}

void PlayerData::SetRotation(Vector3 rot)
{
	this->rotation = rot;
}

Vector3 PlayerData::Respawn()
{
	SetPosition(respawnPosition);
	return respawnPosition;
}

void PlayerData::SetRespawnPosition(Vector3 newPosition)
{
	respawnPosition = newPosition;
}
