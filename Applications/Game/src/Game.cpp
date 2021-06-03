#include <Game.hpp>
#include <Systems/PlayerStateSystem.hpp>
#include <GameEngine/ResourceManager.hpp>
#include <GameEngine/CommonComponents.hpp>
#include <Systems/MovingObstacleSystem.hpp>
#include <Components/ObstacleComponent.hpp>
#include <GameEngine/Systems/PhysicsSystem.hpp>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h> // Immediate-mode GUI

using namespace ECS;
using namespace Utilities;

const float PlayerSize = 7.5f;
const float FloorScale = 8.0f;
const float FloorSize = 14.0f; // pixels

const string DinoTextureBase = "assets/DinoCharacters/sheets/DinoSprites - ";
vector<string> DinoTextures =
{
	"doux",
	"mort",
	"tard",
	"vita"
};
int DinoTextureIndex = 0;

Game::Game(const ApplicationArgs& args) : Application(args)
{
	GetWorld()->GetSystem<PhysicsSystem>()->DrawDebugInfo = true;
	GetWorld()->AddSystem<PlayerStateSystem>();
	GetWorld()->AddSystem<MovingObstacleSystem>()->MoveSpeed = 250;

	InputSystem* input = GetInput();
	// --- INPUT --- //
	input->Map(KEY_W, "PlayerJump");
	input->Map(KEY_UP, "PlayerJump");
	input->Map(KEY_S, "PlayerCrouch", InputBindingState::DownOrHeld);
	input->Map(KEY_DOWN, "PlayerCrouch", InputBindingState::DownOrHeld);
	input->Map(KEY_S, "PlayerRunning", InputBindingState::Up);
	input->Map(KEY_DOWN, "PlayerRunning", InputBindingState::Up);

	input->Map(KEY_ESCAPE, "KeyQuit", [&](DataStream) { Quit(); });

	// Set default state
	Events()->Send("PlayerRunning");

	CreateFloors();
	CreatePlayer();

	// Pre-load assets on main thread
	PreloadAssets();

	input->Map(MOUSE_BUTTON_LEFT, "SpawnObstacle", [&](const DataStream&)
		{
			Entity obstacle = GetWorld()->CreateEntity();
			obstacle.AddComponent<ObstacleComponent>();

			Vector2 resolution = GetResolution();

			auto sprite = obstacle.AddComponent<AnimatedSpriteComponent>();
			sprite->Sprite = ResourceManager::LoadTexture("assets/kenney_pixelplatformer/Tilemap/characters.png");
			sprite->MaxFrames = 3;
			sprite->TimeBetweenFrames = 200;
			sprite->ReferenceSize =
			{
				0,
				25,
				25,
				25
			};

			auto obstacleTransform = obstacle.AddComponent<TransformComponent>();
			obstacleTransform->Position = { resolution.x, m_Player.GetComponent<TransformComponent>()->Position.y - PlayerSize * 15.0f };
			obstacleTransform->Scale = { PlayerSize * sprite->ReferenceSize.width, PlayerSize * sprite->ReferenceSize.height };

			auto pbody = obstacle.AddComponent<PhysicsBodyComponent>();
			pbody->Scale = { 0.4f, 0.5f };
			pbody->Offset = { 0, 20 };

			/*
			auto soundComponent = obstacle.AddComponent<AudioComponent>();
			soundComponent->Sound = obstacleSound;
			soundComponent->EndAction = AudioEndAction::Remove;
			*/
		}, InputBindingState::Down);

	GAME_LOG_DEBUG("Total entity count: " + to_string(GetWorld()->GetEntityCount()));
}

void Game::OnUpdate(float deltaTime)
{
	SetWindowTitle(("Game [entities: " + to_string(GetWorld()->GetEntityCount()) + "][DeltaTime: " + to_string(GetFrameTime()) + "]").c_str());
}

void Game::OnDrawGUI()
{
	PROFILE_FN();

	GuiGroupBox({ 10, 50, 130, 180 }, "Dino State");

	if (GuiButton({ 20, 60, 110, 30 }, "Idle")) MessageBus::eventBus()->Send("PlayerIdle");
	if (GuiButton({ 20, 100, 110, 30 }, "Run")) MessageBus::eventBus()->Send("PlayerRunning");
	if (GuiButton({ 20, 140, 110, 30 }, "Hurt")) MessageBus::eventBus()->Send("PlayerHurt");
	if (GuiButton({ 20, 180, 110, 30 }, "Crouch")) MessageBus::eventBus()->Send("PlayerCrouch", DataStream().write((short)InputBindingState::Down));

	GuiLine({ 150, 50, 130, 30 }, "Dino Texture");
	GuiSpinner({ 150, 60, 130, 30 }, "", &DinoTextureIndex, 0, (int)DinoTextures.size() - 1, false);
	DinoTextureIndex = clamp(DinoTextureIndex, 0, (int)DinoTextures.size() - 1);
	auto spriteComp = m_Player.GetComponent<AnimatedSpriteComponent>();
	spriteComp->Sprite = ResourceManager::LoadTexture(DinoTextureBase + DinoTextures[DinoTextureIndex] + ".png");

	DrawFPS(10, 10);

	auto t = m_Player.GetComponent<TransformComponent>();
	DrawCircle(t->Position.x, t->Position.y, 2.5f, BLUE);
}

void Game::CreatePlayer()
{
	PROFILE_FN();

	Vector2 resolution = GetResolution();
	Vector2 dpi = GetWindowScaleDPI();

	// GetWorld()->GetSystem<PhysicsSystem>()->SetGravity(Vector2 { 0, 1.0f });

	// --- PLAYER --- //
	m_Player = GetWorld()->CreateEntity();
	m_Player.AddComponent<PlayerInputComponent>();
	m_Player.AddComponent<DebugNameComponent>()->Name = "Player";

	auto playerTransform = m_Player.AddComponent<TransformComponent>();
	auto playerSprite = m_Player.AddComponent<AnimatedSpriteComponent>();

	playerSprite->Sprite = ResourceManager::LoadTexture(DinoTextureBase + DinoTextures[DinoTextureIndex] + ".png");
	Texture texture = ResourceManager::GetTexture(playerSprite->Sprite);
	playerSprite->MaxFrames = 4;
	playerSprite->ReferenceSize = { 0, 0, texture.width / 24.0f, (float)texture.height };

	playerTransform->Position =
	{
		200,
		resolution.y - FloorSize * FloorScale * 1.825f
	};
	playerTransform->ZIndex = 99;
	playerTransform->Scale = { PlayerSize * texture.height, PlayerSize * texture.height };

	auto physicsBody = m_Player.AddComponent<PhysicsBodyComponent>();
	physicsBody->Scale = { 0.7f, 0.65f };

	static const Vector2 RunningScale = { 0.6f, 0.65f };
	static const Vector2 CrouchScale = { 0.7f, 0.5f };
	static const Vector2 CrouchOffset = { 20.0f, 10.0f };
	Events()->AddReceiver("PlayerCrouch", [=](DataStream stream)
		{
			if ((InputBindingState)stream.read<short>() != InputBindingState::Down)
				return;
			physicsBody->Scale = CrouchScale;
			physicsBody->Offset = CrouchOffset;
		});
	Events()->AddReceiver("PlayerRunning", [=](DataStream stream)
		{
			physicsBody->Scale = RunningScale;
			physicsBody->Offset = { 0, 0 };
		});

	Events()->AddReceiver("PhysicsCollision", [&](DataStream stream)
	{
		if (stream.read<EntityID>() != GetWorld()->ID())
			return; // Not for us?

		EntityID a = stream.read<EntityID>();
		EntityID b = stream.read<EntityID>();

		if (a == m_Player || b == m_Player)
			Events()->Send("PlayerHurt");
		else
			return;

		AudioComponent* audio = nullptr;
		if(a == m_Player) audio = GetWorld()->AddComponent<AudioComponent>(b);
		else audio = GetWorld()->AddComponent<AudioComponent>(a);

		if(audio->Sound != InvalidResourceID)
			return; // Already has audio component

		audio->Sound = ResourceManager::LoadSound("assets/Sounds/monke_1.wav");
		audio->EndAction = AudioEndAction::Remove;
	});

	Events()->AddReceiver("PhysicsCollisionEnd", [&](DataStream stream)
	{
		if(stream.read<EntityID>() != GetWorld()->ID() || // Not for us?
			stream.read<EntityID>() != m_Player)
			return;

		if(GetWorld()->GetSystem<PlayerStateSystem>()->GetCurrentState() ==
				PlayerStateSystem::PlayerState::Hurt)
			Events()->Send("PlayerRunning", DataStream().write((short)InputBindingState::Down));
	});
}

static const string FloorTilemap = "assets/kenney_pixelplatformer/Tilemap/tiles_packed.png";
void Game::CreateFloors()
{
	PROFILE_FN();

	Vector2 resolution = GetResolution();

	static const int FloorSpriteCount = 100; // 20;
	static const int FloorSpriteDepth = 10;

	// Create enough memory for the floor sprites
	GetWorld()->PrepareEntities(FloorSpriteCount * FloorSpriteDepth);
	GetWorld()->GetComponentManager()->Prepare<TransformComponent>(FloorSpriteCount * FloorSpriteDepth);
	GetWorld()->GetComponentManager()->Prepare<AnimatedSpriteComponent>(FloorSpriteCount * FloorSpriteDepth);

	float initialX = 0;
	float floorY = resolution.y - (FloorSize * FloorScale) * 0.8f;
	for (int i = 0; i < FloorSpriteCount; i++)
	{
		Entity floor = GetWorld()->CreateEntity();
		// floor.AddComponent<PhysicsBodyComponent>()->Dynamic = false;

		auto floorTransform = floor.AddComponent<TransformComponent>();
		floorTransform->Position = { initialX + i * FloorSize * FloorScale, floorY };
		floorTransform->Scale = { FloorSize * FloorScale, FloorSize * FloorScale };

		auto floorSprite = floor.AddComponent<AnimatedSpriteComponent>();
		floorSprite->Sprite = ResourceManager::LoadTexture(FloorTilemap);
		floorSprite->Frame = i % 3;
		floorSprite->MaxFrames = 3;
		floorSprite->TimeBetweenFrames = 150;

		floorSprite->ReferenceSize =
		{
			20,
			2,
			14,
			14
		};

		for (int j = 0; j < FloorSpriteDepth; j++)
		{
			Entity depth = GetWorld()->CreateEntity();
			auto depthTransform = depth.AddComponent<TransformComponent>();
			depthTransform->Position.y = floorY + (j + 1) * FloorSize * FloorScale;
			depthTransform->Position.x = floorTransform->Position.x;
			depthTransform->Scale = floorTransform->Scale;

			auto depthSprite = depth.AddComponent<AnimatedSpriteComponent>();
			depthSprite->Sprite = ResourceManager::LoadTexture(FloorTilemap);
			depthSprite->Frame = (i + j + 1) % 3;
			depthSprite->MaxFrames = 3;
			depthSprite->TimeBetweenFrames = floorSprite->TimeBetweenFrames;
			depthSprite->ReferenceSize =
			{
				20,
				126,
				14,
				14
			};
		}
	}
}

void Game::PreloadAssets()
{
	ResourceManager::LoadSound("assets/Sounds/monke_1.wav");
	ResourceManager::LoadSound("assets/Sounds/Marcus_Poggers.wav");

	ResourceManager::LoadTexture(FloorTilemap);
	ResourceManager::LoadTexture("assets/kenney_pixelplatformer/Tilemap/characters.png");
}