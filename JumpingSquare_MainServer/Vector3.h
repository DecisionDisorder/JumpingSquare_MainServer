#pragma once

namespace dedi
{
	/// <summary>
	/// 3���� ��ǥ �����͸� �����ϴ� ����ü
	/// </summary>
	struct Vector3
	{
	public:
		float x;
		float y;
		float z;

	public:
		struct Vector3();
		struct Vector3(float x, float y, float z);
	};
}