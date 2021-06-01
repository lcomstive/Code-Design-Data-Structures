#pragma once
#include <vector>
#include <map>
#include <ECS/system.hpp>
#include <ECS/entity.hpp>

class AudioSystem : public ECS::System
{
public:
	void Init() override;

	void Update(float deltaTime) override;

private:
	// EntityID <-> AudioComponent's time until finish
	std::map<ECS::EntityID, float> m_PlayingAudio;
};
