#pragma once
#include <raymath.h>
#include <Physics/AABB.hpp>

struct PhysicsBodyComponent
{
	AABB Collider;
	float Mass = 1.0f;
	bool Dynamic = true;
	Vector2 Force = { 0, 0 }; // Gets applied to physics, then reset, every frame
};