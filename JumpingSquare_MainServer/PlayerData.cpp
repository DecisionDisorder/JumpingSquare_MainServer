#include "PlayerData.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace dedi
{
	PlayerData::PlayerData(rapidjson::Document& document)
	{
		// Json Document로부터 값을 불러와서 멤버변수 초기화
		timestamp = document["timestamp"].GetInt64();
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

	void PlayerData::ApplyData(PlayerData player)
	{
		position = player.position;
		rotation = player.rotation;
		timestamp = player.timestamp;
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
}