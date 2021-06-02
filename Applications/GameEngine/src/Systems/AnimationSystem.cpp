#include <GameEngine/Systems/AnimationSystem.hpp>

#include <map>
#include <ECS/world.hpp>
#include <GameUtilities/messagebus.hpp>
#include <GameEngine/Components/AnimatedSpriteComponent.hpp>

using namespace ECS;
using namespace Utilities;

void AnimationSystem::Init()
{
	MessageBus::eventBus()->AddReceiver("ComponentAddAnimatedSpriteComponent", [&](DataStream stream)
	{
		auto worldID = stream.read<EntityID>();
		auto entityID = stream.read<EntityID>();
		auto sprite = GetWorld()->GetComponent<AnimatedSpriteComponent>(entityID);

		if(m_Timings.find(entityID) != m_Timings.end())
			m_Timings.emplace(entityID, (float)sprite->TimeBetweenFrames);
	});

	MessageBus::eventBus()->AddReceiver("ComponentRemoveAnimatedSpriteComponent", [&](DataStream stream)
	{
		auto worldID = stream.read<EntityID>();
		auto entityID = stream.read<EntityID>();

		if(m_Timings.find(entityID) != m_Timings.end())
			m_Timings.erase(entityID);
	});

	GAME_LOG_DEBUG("Animation system ready");
}

void AnimationSystem::Update(float deltaTime)
{
	PROFILE_FN();
	
	auto animatedSprites = GetWorld()->GetComponents<AnimatedSpriteComponent>();

	for (auto& pair : animatedSprites)
	{
		m_Timings[pair.first] -= deltaTime * 1000.0f;
		if(m_Timings[pair.first] > 0)
			continue;

		pair.second->Frame++;
		if(pair.second->Frame >= pair.second->MaxFrames)
			pair.second->Frame = 0;

		m_Timings[pair.first] = (float)pair.second->TimeBetweenFrames;
	}	
}
