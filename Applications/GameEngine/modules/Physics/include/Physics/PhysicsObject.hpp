#pragma once
#include <raylib.h>
#include <ECS/entity.hpp>

namespace Physics
{
	struct PhysicsObject
	{
		float Mass;
		Vector2 Velocity;

		ECS::EntityID EntityID;
	};
}