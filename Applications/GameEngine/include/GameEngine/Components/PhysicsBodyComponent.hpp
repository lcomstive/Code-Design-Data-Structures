#pragma once
#include <raymath.h>

struct PhysicsBodyComponent
{
	bool Dynamic = true;
	Vector2 Force = { 0, 0 }; // Gets applied to physics, then reset, every frame
};