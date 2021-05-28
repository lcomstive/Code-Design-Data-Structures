#pragma once
#include <ECS/system.hpp>

class SpriteRenderSystem : public ECS::System
{
public:
	void Init() override;
	void Update(float deltaTime) override;
};