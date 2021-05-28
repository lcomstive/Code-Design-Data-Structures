#pragma once
#include <unordered_map>
#include <ECS/system.hpp>

class AnimationSystem : public ECS::System
{
public:
	void Init() override;
	void Update(float deltaTime) override;

private:
	std::unordered_map<unsigned int, float> m_Timings;
};
