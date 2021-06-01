#pragma once
#include <map>
#include <ECS/system.hpp>
#include <raylib.h>

class SpriteRenderSystem : public ECS::System
{
	std::map<unsigned int, Camera2D> m_Cameras;
	
public:
	void Init() override;
	void Draw() override;
};
