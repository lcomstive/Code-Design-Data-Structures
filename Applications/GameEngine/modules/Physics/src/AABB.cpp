#include <Physics/AABB.hpp>

AABB::AABB() : AABB(0, 0, 0, 0) { }
AABB::AABB(Vector2 min, Vector2 max) : Min(min), Max(max) { }
AABB::AABB(const AABB& other) : Min(other.Min), Max(other.Max) { } // Copy constructor
AABB::AABB(float minX, float minY, float width, float height) :
		Min({ minX, minY }),
		Max({ minX + width, minY + height }) { }

float AABB::width() { return Max.x - Min.x; }
float AABB::height() { return Max.y - Min.y; }

Vector2 AABB::center() { return (Min + Max) / 2.0f; }

bool AABB::IsPointInside(Vector2& point)
{
	return  point.x >= Min.x && point.x <= Max.x &&
			point.y >= Min.y && point.y <= Max.y;
}

bool AABB::Intersects(AABB& other)
{
	if(Max.x < other.Min.x || Min.x > other.Max.x) return false;
	if(Max.y < other.Min.y || Min.y > other.Max.y) return false;
	return true;
}

AABB AABB::FromCenter(Vector2 center, Vector2 size)
{
	AABB aabb(
		center.x - (size.x / 2.0f),
		center.y - (size.y / 2.0f),
		size.x, // Width
		size.y  // Height
	);
	return aabb;
}
AABB AABB::FromCenter(Vector2 center, float width, float height) { return FromCenter(center, Vector2 { width, height }); }