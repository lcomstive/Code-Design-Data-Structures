#include <Systems/SpriteRenderSystem.hpp>

#include <raylib.h>
#include <ECS/world.hpp>
#include <Components/SpriteComponent.hpp>
#include <Components/AnimatedSpriteComponent.hpp>
#include <ECS/components/transform-component.hpp>

using namespace ECS;

struct SpriteDrawInfo
{
	Color Tint;
	float Rotation;
	ResourceID Sprite;
	Rectangle DestRect;
	Rectangle SourceRect;
};

void SpriteRenderSystem::Init() { GAME_LOG_DEBUG("Sprite render system ready"); }

void SpriteRenderSystem::Update(float deltaTime)
{
	PROFILE_FN();

	auto entities = world()->GetComponents<TransformComponent, SpriteComponent>();
	auto animatedEntities = world()->GetComponents<TransformComponent, AnimatedSpriteComponent>();

	map<float, SpriteDrawInfo> sortedDrawInfo;

	for (auto& pair : entities)
	{
		TransformComponent* transform = pair.second.first;
		SpriteComponent* sprite = pair.second.second;

		sortedDrawInfo.emplace(transform->Position.z,
			SpriteDrawInfo
			{
				sprite->Tint,
				transform->Rotation,
				sprite->Sprite,
				{
					transform->Position.x,
					transform->Position.y,
					sprite->ReferenceSize.width * transform->Scale.x,
					sprite->ReferenceSize.height * transform->Scale.y
				},
				sprite->ReferenceSize
			});
	}

	for (auto& pair : animatedEntities)
	{
		TransformComponent* transform = pair.second.first;
		AnimatedSpriteComponent* sprite = pair.second.second;

		Texture texture = ResourceManager::GetTexture(sprite->Sprite);
		if (texture.width <= 0 || texture.height <= 0)
			continue; // Texture isn't found?

		Rectangle srcRect = sprite->ReferenceSize;

		srcRect.x += sprite->ReferenceSize.width * sprite->Frame;
		if (srcRect.x >= texture.width)
		{
			srcRect.x -= texture.width;
			srcRect.y += sprite->ReferenceSize.height;
		}

		sortedDrawInfo.emplace(transform->Position.z,
			SpriteDrawInfo
			{
				sprite->Tint,
				transform->Rotation,
				sprite->Sprite,
				{
					transform->Position.x,
					transform->Position.y,
					sprite->ReferenceSize.width * transform->Scale.x,
					sprite->ReferenceSize.height * transform->Scale.y
				},
				srcRect
			});
	}

	for (auto& drawInfo : sortedDrawInfo)
	{
		if(drawInfo.second.Sprite == InvalidResourceID)
			continue;
		
		Texture texture = ResourceManager::GetTexture(drawInfo.second.Sprite);
		if (texture.width <= 0 || texture.height <= 0)
			continue; // Texture isn't found?

		SpriteDrawInfo info = drawInfo.second;
		DrawTexturePro(
			texture,
			info.SourceRect,
			info.DestRect,
			{
				info.DestRect.width / 2.0f,
				info.DestRect.height / 2.0f
			},
			info.Rotation,
			info.Tint
		);
	}
}
