#pragma once

/// <summary>
/// 3차원 좌표 데이터를 보관하는 구조체
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