#pragma once
#include "Vector3.h"
#include "rapidjson/document.h"
#include <string>

namespace dedi
{
	/// <summary>
	/// �÷��̾��� �ΰ��� ������ Ŭ����
	/// </summary>
	class PlayerData
	{
	public:
		PlayerData(rapidjson::Document& document);

		/// <summary>
		/// �÷��̾� �̸��� �ҷ��´�
		/// </summary>
		inline std::string getPlayerName() const { return _name; }
		/// <summary>
		/// �÷��̾� ȸ�� ���� �ҷ��´�
		/// </summary>
		inline Vector3 getRotation() const { return _rotation; }
		/// <summary>
		/// �÷��̾� ��ġ�� �ҷ��´�
		/// </summary>
		inline Vector3 getPosition() const { return _position; }
		/// <summary>
		/// �÷��̾� ���� ���θ� �ҷ��´�
		/// </summary>
		/// <returns></returns>
		inline bool isAlive() const { return _alive; }
		/// <summary>
		/// �÷��̾� ���� ���� ����
		/// </summary>
		/// <param name="alive">���� ����</param>
		inline void setAlive(const bool alive) { _alive = alive; }

		/// <summary>
		/// �÷��̾� �����͸� �����Ͽ� �����Ѵ�.
		/// </summary>
		/// <param name="player">������ ������</param>
		void applyData(const PlayerData& player);
	
		/// <summary>
		/// �÷��̾� ������ ó��
		/// </summary>
		/// <returns>������ ��ġ</returns>
		Vector3 respawn();

		/// <summary>
		/// ������ ��ġ ����
		/// </summary>
		/// <param name="newPosition">���ο� ������ ��ġ</param>
		void setRespawnPosition(const Vector3& newPosition);
		/// <summary>
		/// ������ ��ġ�� �ҷ��´�
		/// </summary>
		inline Vector3 getRespawnPosition() const { return _respawnPosition; }

		/// <summary>
		/// �÷��̾� ��ġ ����
		/// </summary>
		/// <param name="pos">���ο� ��ġ</param>
		void setPosition(const Vector3& pos);
		/// <summary>
		/// �÷��̾� ȸ�� �� ����
		/// </summary>
		/// <param name="rot">���ο� ȸ�� ��</param>
		void setRotation(const Vector3& rot);

	private:
		// Ŭ���̾�Ʈ���� ���Ź��� �ð� (unix epoch time)
		long long _timestamp;
		// �÷��̾� �̸�
		std::string _name;
		// �÷��̾� ��ġ
		Vector3 _position;
		// �÷��̾� ȸ�� ��
		Vector3 _rotation;
		// ������ ��ġ
		Vector3 _respawnPosition;
		// ���� ����
		bool _alive;
	};
}
