#include "PlayerData.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace dedi
{
	PlayerData::PlayerData(rapidjson::Document& document)
	{
		// Json Document로부터 값을 불러와서 멤버변수 초기화
		_timestamp = document["timestamp"].GetInt64();
		_name = std::string(document["playerName"].GetString());
		_position = Vector3(document["positionX"].GetDouble(), document["positionY"].GetDouble(), document["positionZ"].GetDouble());
		_rotation = Vector3(document["rotationX"].GetDouble(), document["rotationY"].GetDouble(), document["rotationZ"].GetDouble());
		_alive = document["alive"].GetBool();
		_respawnPosition = Vector3(0.f, 1.0f, 0.f);

	}

	void PlayerData::setPosition(const Vector3& position)
	{
		_position = position;
	}

	void PlayerData::setRotation(const Vector3& rot)
	{
		_rotation = rot;
	}

	void PlayerData::applyData(const PlayerData& player)
	{
		_position = player._position;
		_rotation = player._rotation;
		_timestamp = player._timestamp;
	}

	Vector3 PlayerData::respawn()
	{
		setPosition(_respawnPosition);
		return _respawnPosition;
	}

	void PlayerData::setRespawnPosition(const Vector3& newPosition)
	{
		_respawnPosition = newPosition;
	}
}