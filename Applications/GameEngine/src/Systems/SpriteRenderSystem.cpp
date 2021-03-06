#include <GameEngine/Systems/SpriteRenderSystem.hpp>

#include <raylib.h>
#include <ECS/world.hpp>
#include <GameUtilities/math.hpp>
#include <GameUtilities/messagebus.hpp>
#include <DataStructures/QuickSort.hpp>
#include <ECS/components/transform-component.hpp>
#include <GameEngine/Components/SpriteComponent.hpp>
#include <GameEngine/Components/CameraComponent.hpp>
#include <GameEngine/Components/AnimatedSpriteComponent.hpp>

#include <rlgl.h>

using namespace ECS;
using namespace LCDS;
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
			if (worldID != GetWorld()->ID())
				return; // Not for this system
			EntityID entity = stream.read<EntityID>();

			Camera2D camera = { 0 };
			CameraComponent* component = GetWorld()->GetComponent<CameraComponent>(entity);
			camera.zoom = component->Zoom;
			auto camTransform = GetWorld()->GetComponent<TransformComponent>(entity);
			if (camTransform)
				camera.target = { camTransform->Position.x, camTransform->Position.y };
			m_Cameras.emplace(entity, camera);
		});

	MessageBus::eventBus()->AddReceiver("ComponentRemove" + CameraComponentName, [&](DataStream stream)
		{
			EntityID worldID = stream.read<EntityID>();
			if (worldID != GetWorld()->ID())
				return; // Not for this system
			m_Cameras.erase(stream.read<EntityID>());
		});

	GAME_LOG_DEBUG("Sprite render system ready");
}

void SpriteRenderSystem::Draw()
{
	PROFILE_FN();

	auto entities = GetWorld()->GetComponents<TransformComponent, SpriteComponent>();
	auto animatedEntities = GetWorld()->GetComponents<TransformComponent, AnimatedSpriteComponent>();

	vector<SpriteDrawInfo> sortedDrawInfo;

	for (auto& pair : entities)
	{
		TransformComponent* transform = pair.second.first;
		SpriteComponent* sprite = pair.second.second;

		Vector2 spriteSize = transform->Scale;
		/*
		spriteSize.x *= sprite->ReferenceSize.width;
		spriteSize.y *= sprite->ReferenceSize.height;
		*/

		sortedDrawInfo.push_back(
			SpriteDrawInfo
			{
				sprite->Tint,
				transform->Rotation,
				sprite->Sprite,
				{
					transform->Position.x,
					transform->Position.y,
					spriteSize.x,
					spriteSize.y
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

		Vector2 spriteSize = transform->Scale;
		/*
		spriteSize.x *= sprite->ReferenceSize.width;
		spriteSize.y *= sprite->ReferenceSize.height;
		*/

		sortedDrawInfo.push_back(
			SpriteDrawInfo
			{
				sprite->Tint,
				transform->Rotation,
				sprite->Sprite,
				{
					transform->Position.x,
					transform->Position.y,
					spriteSize.x,
					spriteSize.y
				},
				srcRect
			});
	}

	// TODO: Sort sortedDrawInfo based on transformComponent->Position.z

	Vector2 dpi = GetWindowScaleDPI();
	Vector2 camOffset =
	{
			(GetScreenWidth() / 2.0f),
			(GetScreenHeight() / 2.0f)
	};
	for (auto& pair : m_Cameras)
	{
		Camera2D& cam = pair.second;
		CameraComponent* comp = GetWorld()->GetComponent<CameraComponent>(pair.first);

		cam.target = GetWorld()->GetComponent<TransformComponent>(pair.first)->Position;
		// cam.offset = camOffset; // Transform so center of screen is (0, 0)
		Vector2 camCenter = camOffset - cam.target;

		BeginMode2D(cam);

		rlViewport(
			(int)(comp->Viewport.x * dpi.x),
			(int)(comp->Viewport.y * dpi.y),
			(int)comp->Viewport.width,
			(int)comp->Viewport.height);

		for (auto& drawInfo : sortedDrawInfo)
		{
			if (drawInfo.Sprite == InvalidResourceID)
				continue;

			Texture texture = ResourceManager::GetTexture(drawInfo.Sprite);
			if (texture.width <= 0 || texture.height <= 0)
				continue; // Texture isn't found?

			Vector2 center =
			{
				drawInfo.DestRect.x,
				drawInfo.DestRect.y,
			};
			Vector2 distance = camCenter - center;
			
			if(abs(distance.x) > (comp->Viewport.width / 1.75f) || // Cull out of x bounds
			   abs(distance.y) > (comp->Viewport.height / 1.75f))  // Cull out of y bounds
				continue; // Consider yourself culled

			DrawTexturePro(
				texture,
				drawInfo.SourceRect,
				drawInfo.DestRect,
				{
					drawInfo.DestRect.width / 2.0f,
					drawInfo.DestRect.height / 2.0f,
				},
				drawInfo.Rotation,
				drawInfo.Tint
			);
		}

		EndMode2D();
	}

	rlViewport(0, 0, (int)(GetScreenWidth() * dpi.x), (int)(GetScreenHeight() * dpi.y));
}
