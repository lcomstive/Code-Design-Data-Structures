#pragma once
#include <raylib.h>
#include <ECS/entity.hpp>
#include <Physics/AABB.hpp>

namespace Physics
{
	struct PhysicsObject
	{
		float Mass;
		bool Static;
		bool Trigger;
		AABB Collider;
		Vector2 Velocity;

		PhysicsObject() :
			Mass(1.0f),
			Static(false),
			Trigger(false),
			Collider(AABB(0, 0, 0, 0)),
			Velocity({ 0, 0 })
		{ }
	};
}