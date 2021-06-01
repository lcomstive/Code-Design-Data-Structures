#pragma once
#include <raylib.h>
#include <GameEngine/ResourceManager.hpp>

struct AnimatedSpriteComponent
{
	Color		 Tint;
	unsigned int Frame;
	ResourceID	 Sprite;
	unsigned int MaxFrames;
	Rectangle	 ReferenceSize; // (x, y) offset; (x, y) size (e.g. for spritesheet)
	unsigned int TimeBetweenFrames; // Milliseconds

	AnimatedSpriteComponent() : AnimatedSpriteComponent(InvalidResourceID) { }
	AnimatedSpriteComponent(const ResourceID& sprite) :
		Frame(0),
		MaxFrames(0),
		Sprite(sprite),
		Tint(RAYWHITE),
		TimeBetweenFrames(100)
	{
		Texture texture = ResourceManager::GetTexture(Sprite);
		if(texture.width <= 0 || texture.height <= 0)
		{
			Sprite = InvalidResourceID;
			ReferenceSize = { 0, 0, 0, 0 };
			return;
		}

		ReferenceSize =
		{
			0, 0,
			(float)texture.width,
			(float)texture.height
		};
	}

	void Subdivide(int subWidth, int subHeight = 1)
	{
		if(Sprite == InvalidResourceID)
			return;
		Texture texture = ResourceManager::GetTexture(Sprite);
		if(texture.width <= 0 || texture.height <= 0)
			return;
		
		MaxFrames = subWidth;
		ReferenceSize.width = texture.width   / (float)subWidth;
		ReferenceSize.height = texture.height / (float)subHeight;
	}
};