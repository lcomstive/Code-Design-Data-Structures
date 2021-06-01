#pragma once
#include <raymath.h>

namespace ECS
{
	struct TransformComponent
	{
		Vector2 Scale;
		float Rotation;
		Vector3 Position;

		TransformComponent() : Rotation(0), Scale({ 1, 1 }), Position( { 0, 0, 0 }) { }
	};
}