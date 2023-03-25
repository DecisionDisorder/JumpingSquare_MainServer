#pragma once
#include "Vector3.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>

class PlayerData
{
private:
	long long timestamp;
	std::string name;
	Vector3 position;
	Vector3 rotation;
	Vector3 respawnPosition;
	bool alive;

public:
	PlayerData(rapidjson::Document& document);

	inline std::string GetPlayerName() { return name; }
	inline Vector3 GetRotation() { return rotation; }
	inline Vector3 GetPosition() { return position; }
	inline bool IsAlive() { return alive; }
	inline void SetAlive(bool alive) { this->alive = alive; }

	void ApplyData(PlayerData player);

	Vector3 Respawn();

	void SetRespawnPosition(Vector3 newPosition);
	inline Vector3 GetRespawnPosition() { return respawnPosition; }

	void SetPosition(Vector3 pos);
	void SetRotation(Vector3 rot);
};
