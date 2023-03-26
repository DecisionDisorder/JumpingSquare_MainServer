#pragma once
#include "Vector3.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>

/// <summary>
/// �÷��̾��� �ΰ��� ������ Ŭ����
/// </summary>
class PlayerData
{
private:
	// Ŭ���̾�Ʈ���� ���Ź��� �ð� (unix epoch time)
	long long timestamp;
	// �÷��̾� �̸�
	std::string name;
	// �÷��̾� ��ġ
	Vector3 position;
	// �÷��̾� ȸ�� ��
	Vector3 rotation;
	// ������ ��ġ
	Vector3 respawnPosition;
	// ���� ����
	bool alive;

public:
	PlayerData(rapidjson::Document& document);

	/// <summary>
	/// �÷��̾� �̸��� �ҷ��´�
	/// </summary>
	inline std::string GetPlayerName() { return name; }
	/// <summary>
	/// �÷��̾� ȸ�� ���� �ҷ��´�
	/// </summary>
	inline Vector3 GetRotation() { return rotation; }
	/// <summary>
	/// �÷��̾� ��ġ�� �ҷ��´�
	/// </summary>
	inline Vector3 GetPosition() { return position; }
	/// <summary>
	/// �÷��̾� ���� ���θ� �ҷ��´�
	/// </summary>
	/// <returns></returns>
	inline bool IsAlive() { return alive; }
	/// <summary>
	/// �÷��̾� ���� ���� ����
	/// </summary>
	/// <param name="alive">���� ����</param>
	inline void SetAlive(bool alive) { this->alive = alive; }

	/// <summary>
	/// �÷��̾� �����͸� �����Ͽ� �����Ѵ�.
	/// </summary>
	/// <param name="player">������ ������</param>
	void ApplyData(PlayerData player);
	
	/// <summary>
	/// �÷��̾� ������ ó��
	/// </summary>
	/// <returns>������ ��ġ</returns>
	Vector3 Respawn();

	/// <summary>
	/// ������ ��ġ ����
	/// </summary>
	/// <param name="newPosition">���ο� ������ ��ġ</param>
	void SetRespawnPosition(Vector3 newPosition);
	/// <summary>
	/// ������ ��ġ�� �ҷ��´�
	/// </summary>
	inline Vector3 GetRespawnPosition() { return respawnPosition; }

	/// <summary>
	/// �÷��̾� ��ġ ����
	/// </summary>
	/// <param name="pos">���ο� ��ġ</param>
	void SetPosition(Vector3 pos);
	/// <summary>
	/// �÷��̾� ȸ�� �� ����
	/// </summary>
	/// <param name="rot">���ο� ȸ�� ��</param>
	void SetRotation(Vector3 rot);
};
