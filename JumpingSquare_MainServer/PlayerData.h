#pragma once
#include "Vector3.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>

/// <summary>
/// 플레이어의 인게임 데이터 클래스
/// </summary>
class PlayerData
{
private:
	// 클라이언트에서 수신받은 시간 (unix epoch time)
	long long timestamp;
	// 플레이어 이름
	std::string name;
	// 플레이어 위치
	Vector3 position;
	// 플레이어 회전 값
	Vector3 rotation;
	// 리스폰 위치
	Vector3 respawnPosition;
	// 생존 여부
	bool alive;

public:
	PlayerData(rapidjson::Document& document);

	/// <summary>
	/// 플레이어 이름을 불러온다
	/// </summary>
	inline std::string GetPlayerName() { return name; }
	/// <summary>
	/// 플레이어 회전 값을 불러온다
	/// </summary>
	inline Vector3 GetRotation() { return rotation; }
	/// <summary>
	/// 플레이어 위치를 불러온다
	/// </summary>
	inline Vector3 GetPosition() { return position; }
	/// <summary>
	/// 플레이어 생존 여부를 불러온다
	/// </summary>
	/// <returns></returns>
	inline bool IsAlive() { return alive; }
	/// <summary>
	/// 플레이어 생존 여부 지정
	/// </summary>
	/// <param name="alive">생존 여부</param>
	inline void SetAlive(bool alive) { this->alive = alive; }

	/// <summary>
	/// 플레이어 데이터를 복사하여 적용한다.
	/// </summary>
	/// <param name="player">적용할 데이터</param>
	void ApplyData(PlayerData player);
	
	/// <summary>
	/// 플레이어 리스폰 처리
	/// </summary>
	/// <returns>리스폰 위치</returns>
	Vector3 Respawn();

	/// <summary>
	/// 리스폰 위치 지정
	/// </summary>
	/// <param name="newPosition">새로운 리스폰 위치</param>
	void SetRespawnPosition(Vector3 newPosition);
	/// <summary>
	/// 리스폰 위치를 불러온다
	/// </summary>
	inline Vector3 GetRespawnPosition() { return respawnPosition; }

	/// <summary>
	/// 플레이어 위치 지정
	/// </summary>
	/// <param name="pos">새로운 위치</param>
	void SetPosition(Vector3 pos);
	/// <summary>
	/// 플레이어 회전 값 지정
	/// </summary>
	/// <param name="rot">새로운 회전 값</param>
	void SetRotation(Vector3 rot);
};
