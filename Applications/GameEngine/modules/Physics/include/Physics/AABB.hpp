#pragma once
#include <GameUtilities/math.hpp>

struct AABB
{
	Vector2 Min;
	Vector2 Max;

	AABB();
	AABB(Vector2 min, Vector2 max);
	AABB(const AABB& other);// Copy constructor
	AABB(float minX, float minY, float width, float height);

	float width();
	float height();

	Vector2 center();

	bool Intersects(AABB& other);
	bool IsPointInside(Vector2& point);

	static AABB FromCenter(Vector2 center, Vector2 size);
	static AABB FromCenter(Vector2 center, float width, float height);
};