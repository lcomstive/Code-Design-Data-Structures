#pragma once
#include <raymath.h>

namespace ECS
{
	struct TransformComponent
	{
		Vector2 Scale;
		float Rotation;
		Vector2 Position;
		float ZIndex;

		TransformComponent() : ZIndex(0), Rotation(0), Scale({ 1, 1 }), Position( { 0, 0 }) { }
	};
}