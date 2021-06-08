#include <ECS/world.hpp>
#include <GameEngine/Systems/AudioSystem.hpp>
#include <GameEngine/Components/AudioComponent.hpp>

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
			if (worldID != GetWorld()->ID())
				return; // Event not in this GetWorld/scene
			EntityID entity = stream.read<EntityID>();
			AudioComponent* audio = GetWorld()->GetComponent<AudioComponent>(entity);

			if (m_PlayingAudio.find(entity) != m_PlayingAudio.end())
				return;

			// WARNING: Getting sound via ResourceManager after adding component requires MessageBus to buffer messages.
			//			Otherwise audioComponent->Sound is invalid
			Sound sound = ResourceManager::GetSound(audio->Sound);
			m_PlayingAudio.emplace(entity, GetSoundDuration(sound));
			PlaySoundMulti(ResourceManager::GetSound(audio->Sound));
		});
	MessageBus::eventBus()->AddReceiver("RemoveComponent" + AudioComponentName, [&](DataStream stream)
		{
			EntityID worldID = stream.read<EntityID>();
			if (worldID != GetWorld()->ID())
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

	auto playingAudio = m_PlayingAudio; // Copy incase of modification
	for(auto it = playingAudio.rbegin(); it != playingAudio.rend(); ++it)
	{
  		AudioComponent* audio = GetWorld()->GetComponent<AudioComponent>(it->first);

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

		float& timeLeft = playingAudio[it->first];
		timeLeft -= deltaTime;
		if (timeLeft > 0)
			continue;

		StopSound(sound);
		switch (audio->EndAction)
		{
		case AudioEndAction::Destroy:
			GetWorld()->DestroyEntity(it->first);
			continue;
		case AudioEndAction::Loop:
			timeLeft = GetSoundDuration(sound);
			PlaySound(sound);
			continue;
		case AudioEndAction::Remove:
			GetWorld()->RemoveComponent<AudioComponent>(it->first);
			continue;
		case AudioEndAction::Stop:
			audio->Playing = false;
			StopSound(sound);
			continue;
		}
	}
}

