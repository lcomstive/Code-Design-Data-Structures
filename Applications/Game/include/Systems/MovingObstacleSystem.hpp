#pragma once
#include <ECS/world.hpp>

class MovingObstacleSystem : public ECS::System
{
public:
	float MoveSpeed = 10.0f;

	void Update(float deltaTime) override;
};