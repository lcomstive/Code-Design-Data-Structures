#pragma once
#include <string>
#include <raylib.h>
#include <GameEngine/ResourceManager.hpp>

enum class AudioEndAction : char
{
	Stop,	// Stops audio
	Loop,	// Restarts audio clip
	Remove,	// Removes audio component from entity
	Destroy // Destroys associated entity
};

struct AudioComponent
{
	ResourceID Sound = InvalidResourceID;
	bool Playing = true;
	AudioEndAction EndAction = AudioEndAction::Destroy;
};