#include "Vector3.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>

class PlayerData
{
private:
	std::string name;
	Vector3 position;
	Vector3 rotation;
	Vector3 velocity;
	Vector3 respawnPosition;
	bool alive;

private:
	void SetPosition(Vector3 position);

public:
	PlayerData(rapidjson::Document& document);

	inline Vector3 GetPosition() { return position; }
	inline bool IsAlive() { return alive; }

	Vector3 Respawn();

	void SetRespawnPosition(Vector3 newPosition);
	inline Vector3 GetRespawnPosition() { return respawnPosition; }
};
