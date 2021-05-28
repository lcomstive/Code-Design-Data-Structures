#include <Systems/AudioSystem.hpp>
#include <Components/AudioComponent.hpp>

#include "ECS/world.hpp"

const string AudioComponentName = typeid(AudioComponent).name();

using namespace ECS;
using namespace Utilities;

inline float GetSoundDuration(Sound& s) { return s.sampleCount / (float)s.stream.sampleRate; }

void AudioSystem::Init()
{
	PROFILE_FN();
	
	GAME_LOG_DEBUG("Initialising audio system");
	InitAudioDevice();

	MessageBus::eventBus()->AddReceiver("ComponentAdd" + AudioComponentName, [&](DataStream stream)
		{
			EntityID worldID = stream.read<EntityID>();
			if (worldID != world()->ID())
				return; // Event not in this world/scene
			EntityID entity = stream.read<EntityID>();
			AudioComponent* audio = world()->GetComponent<AudioComponent>(entity);

			if (m_PlayingAudio.find(entity) != m_PlayingAudio.end())
				return;

			Sound sound = ResourceManager::GetSound(audio->Sound);
			m_PlayingAudio.emplace(entity, GetSoundDuration(sound));
			PlaySoundMulti(sound);
		});
	MessageBus::eventBus()->AddReceiver("RemoveComponent" + AudioComponentName, [&](DataStream stream)
		{
			EntityID worldID = stream.read<EntityID>();
			if (worldID != world()->ID())
				return; // Event not for this system
			EntityID entity = stream.read<EntityID>();
			m_PlayingAudio.erase(entity);
		});

	GAME_LOG_DEBUG("Audio system ready");
}

void AudioSystem::Update(float deltaTime)
{
	if(m_PlayingAudio.size() == 0)
		return;
	
	PROFILE_FN()

	for(auto it = m_PlayingAudio.rbegin(); it != m_PlayingAudio.rend(); ++it)
	{
  		AudioComponent* audio = world()->GetComponent<AudioComponent>(it->first);

		if(!audio)
		{
			m_PlayingAudio.erase(it->first);
			if(m_PlayingAudio.size() == 0)
				return; // No more audio to play
			continue;
		}

		Sound sound = ResourceManager::GetSound(audio->Sound);
		if(sound.sampleCount == 0)
			continue; // Couldn't find sound?
		
 		if (!audio->Playing)
		{
			if(IsSoundPlaying(sound))
				PauseSound(sound);
			continue;
		}

		float& timeLeft = m_PlayingAudio[it->first];
		timeLeft -= deltaTime;
		if (timeLeft > 0)
			continue;

		StopSound(sound);
		switch (audio->EndAction)
		{
		case AudioEndAction::Destroy:
			world()->DestroyEntity(it->first);
			continue;
		case AudioEndAction::Loop:
			timeLeft = GetSoundDuration(sound);
			PlaySound(sound);
			continue;
		case AudioEndAction::Remove:
			world()->RemoveComponent<AudioComponent>(it->first);
			continue;
		case AudioEndAction::Stop:
			audio->Playing = false;
			StopSound(sound);
			continue;
		}
	}
}

