#pragma once
#include <raylib.h>
#include <GameEngine/ResourceManager.hpp>

struct SpriteComponent
{
	Color	  Tint;
	ResourceID Sprite;
	Rectangle ReferenceSize; // (x, y) offset; (x, y) size (e.g. for spritesheet)

	SpriteComponent() : SpriteComponent(InvalidResourceID) { }
	SpriteComponent(const ResourceID& sprite) : Sprite(sprite), Tint(RAYWHITE)
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
};