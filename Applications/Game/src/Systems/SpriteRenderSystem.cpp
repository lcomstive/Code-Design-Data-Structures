#include <Systems/SpriteRenderSystem.hpp>

#include <raylib.h>
#include <ECS/world.hpp>
#include <Components/SpriteComponent.hpp>
#include <Components/CameraComponent.hpp>
#include <Components/AnimatedSpriteComponent.hpp>
#include <ECS/components/transform-component.hpp>

#include <rlgl.h>

using namespace ECS;
using namespace Utilities;

const string CameraComponentName = typeid(CameraComponent).name();

struct SpriteDrawInfo
{
	Color Tint;
	float Rotation;
	ResourceID Sprite;
	Rectangle DestRect;
	Rectangle SourceRect;
};

void SpriteRenderSystem::Init()
{
	MessageBus::eventBus()->AddReceiver("ComponentAdd" + CameraComponentName, [&](DataStream stream)
		{
			EntityID worldID = stream.read<EntityID>();
			if (worldID != world()->ID())
				return; // Not for this system
			EntityID entity = stream.read<EntityID>();

			Camera2D camera = { 0 };
			CameraComponent* component = world()->GetComponent<CameraComponent>(entity);
			Vector3 cameraPos = world()->GetComponent<TransformComponent>(entity)->Position;
			camera.zoom = component->Zoom;
			camera.target = { cameraPos.x, cameraPos.y };
			m_Cameras.emplace(entity, camera);
		});

	MessageBus::eventBus()->AddReceiver("ComponentRemove" + CameraComponentName, [&](DataStream stream)
		{
			EntityID worldID = stream.read<EntityID>();
			if (worldID != world()->ID())
				return; // Not for this system
			m_Cameras.erase(stream.read<EntityID>());
		});

	GAME_LOG_DEBUG("Sprite render system ready");
}

void SpriteRenderSystem::Update(float deltaTime)
{
	PROFILE_FN()

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

	Vector2 cameraScreenOffset =
	{
		GetScreenWidth() / 2.0f,
		GetScreenHeight() / 2.0f
	};
	for (auto& pair : m_Cameras)
	{
		Camera2D& cam = pair.second;
		CameraComponent* comp = world()->GetComponent<CameraComponent>(pair.first);

		Vector3 camPos = world()->GetComponent<TransformComponent>(pair.first)->Position;
		cam.target =
		{
			camPos.x - cameraScreenOffset.x,
			camPos.y - cameraScreenOffset.y
		};

		BeginMode2D(cam);
		rlViewport(
			(int)comp->Viewport.x,
			(int)comp->Viewport.y,
			(int)comp->Viewport.width,
			(int)comp->Viewport.height);

		for (auto& drawInfo : sortedDrawInfo)
		{
			if (drawInfo.second.Sprite == InvalidResourceID)
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
		
		EndMode2D();
	}
	rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());
}
