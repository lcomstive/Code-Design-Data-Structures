#pragma once
#include <raymath.h>

namespace ECS
{
	struct TransformComponent
	{
		float Rotation		= 0;
		Vector2 Scale		= { 1, 1 };
		Vector3 Position	= { 0, 0, 0 };
	};
}