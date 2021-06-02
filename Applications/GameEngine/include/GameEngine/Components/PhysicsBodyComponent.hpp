#pragma once
#include <raymath.h>
#include <Physics/AABB.hpp>

struct PhysicsBodyComponent
{
	float Mass = 1.0f;
	bool Static = false;  // When true, does not get affected by physics forces
	bool Trigger = false; // When true, body does not resolve collisions. Does not get affected by physics forces
	Vector2 Force  = { 0, 0 }; // Gets applied to physics, then reset, every frame
	Vector2 Scale  = { 1, 1 }; // Scale of collider relative to transform scale
	Vector2 Offset = { 0, 0 }; // Offset from center of entity
};