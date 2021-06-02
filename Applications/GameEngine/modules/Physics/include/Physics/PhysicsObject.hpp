#pragma once
#include <raylib.h>
#include <ECS/entity.hpp>
#include <Physics/AABB.hpp>

namespace Physics
{
	struct PhysicsObject
	{
		float Mass;
		AABB Collider;
		Vector2 Velocity;

		PhysicsObject() :
			Mass(1.0f),
			Collider(AABB(0, 0, 0, 0)),
			Velocity({ 0, 0 })
		{ }
	};
}