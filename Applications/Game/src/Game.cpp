#include <Game.hpp>
#include <GameEngine/ResourceManager.hpp>
#include <GameEngine/CommonComponents.hpp>
#include <Systems/MovingObstacleSystem.hpp>
#include <Components/ObstacleComponent.hpp>

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
	GetWorld()->AddSystem<MovingObstacleSystem>()->MoveSpeed = 250;

	InputSystem* input = GetInput();
	// --- INPUT --- //
	input->Map(KEY_W, "PlayerJump");
	input->Map(KEY_UP, "PlayerJump");
	input->Map(KEY_S, "PlayerCrouch", InputBindingState::Held);
	input->Map(KEY_DOWN, "PlayerCrouch", InputBindingState::Held);
	input->Map(KEY_S, "PlayerRunning", InputBindingState::Up);
	input->Map(KEY_DOWN, "PlayerRunning", InputBindingState::Up);

	input->Map(KEY_ESCAPE, "KeyQuit", [&](DataStream) { Quit(); });

	// Set default state
	Events()->Send("PlayerRunning");

	/*
	m_InputSystem->Map(KEY_Q, "PlaySound1", [&](DataStream)
		{
			Entity e = m_World->CreateEntity();
			e.AddComponent<AudioComponent>()
				->Sound = ResourceManager::LoadSound("assets/Sounds/monke_1.wav");
		}, InputBindingState::Down);
	*/

	CreateFloors();
	CreatePlayer();

	ResourceManager::LoadSound("assets/Sounds/monke_1.wav");
	ResourceManager::LoadTexture("assets/kenney_pixelplatformer/Tilemap/characters.png");
	input->Map(MOUSE_BUTTON_LEFT, "SpawnObstacle", [&](DataStream)
	{
		Entity obstacle = GetWorld()->CreateEntity();
		obstacle.AddComponent<ObstacleComponent>();

		Vector2 resolution = GetResolution();
		auto obstacleTransform = obstacle.AddComponent<TransformComponent>();
		obstacleTransform->Position = { resolution.x, resolution.y / 2.0f };
		obstacleTransform->Scale = { PlayerSize, PlayerSize };
		
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

		auto soundComponent = obstacle.AddComponent<AudioComponent>();
		soundComponent->Sound = ResourceManager::LoadSound("assets/Sounds/monke_1.wav");
		soundComponent->EndAction = AudioEndAction::Remove;
	}, InputBindingState::Down);
}

void Game::OnUpdate(float deltaTime) { }

void Game::OnDrawGUI()
{
	PROFILE_FN();

	GuiGroupBox({ 10, 50, 130, 180 }, "Dino State");

	if (GuiButton({ 20, 60, 110, 30 }, "Idle")) MessageBus::eventBus()->Send("PlayerIdle");
	if (GuiButton({ 20, 100, 110, 30 }, "Run")) MessageBus::eventBus()->Send("PlayerRunning");
	if (GuiButton({ 20, 140, 110, 30 }, "Hurt")) MessageBus::eventBus()->Send("PlayerHurt");
	if (GuiButton({ 20, 180, 110, 30 }, "Crouch")) MessageBus::eventBus()->Send("PlayerCrouch");

	GuiLine({ 10, 250, 130, 30 }, "Dino Texture");
	GuiSpinner({ 10, 260, 130, 30 }, "", &DinoTextureIndex, 0, (int)DinoTextures.size() - 1, false);
	DinoTextureIndex = clamp(DinoTextureIndex, 0, (int)DinoTextures.size() - 1);
	m_Player.GetComponent<AnimatedSpriteComponent>()->Sprite = ResourceManager::LoadTexture(DinoTextureBase + DinoTextures[DinoTextureIndex] + ".png");
}

void Game::CreatePlayer()
{
	PROFILE_FN();

	Vector2 resolution = GetResolution();
	Vector2 dpi = GetWindowScaleDPI();

	// --- PLAYER --- //
	m_Player = GetWorld()->CreateEntity();
	m_Player.AddComponent<PlayerInputComponent>();
	m_Player.AddComponent<DebugNameComponent>()->Name = "Player";

	auto playerTransform = m_Player.AddComponent<TransformComponent>();
	playerTransform->Scale.x = playerTransform->Scale.y = PlayerSize;

	auto playerSprite = m_Player.AddComponent<AnimatedSpriteComponent>();
	playerSprite->Sprite = ResourceManager::LoadTexture(DinoTextureBase + DinoTextures[DinoTextureIndex] + ".png");
	Texture texture = ResourceManager::GetTexture(playerSprite->Sprite);
	playerSprite->MaxFrames = 4;
	playerSprite->ReferenceSize = { 0, 0, texture.width / 24.0f, (float)texture.height };

	playerTransform->Position =
	{
		250,
		resolution.y - FloorSize * FloorScale * 1.5f,
		99
	};

	m_Player.AddComponent<PhysicsBodyComponent>();
}

static const string FloorTilemap = "assets/kenney_pixelplatformer/Tilemap/tiles_packed.png";
void Game::CreateFloors()
{
	PROFILE_FN();

	Vector2 resolution = GetResolution();

	static const int FloorSpriteCount = 20;
	static const int FloorSpriteDepth = 10;
	float initialX = 0;
	float floorY = resolution.y - (FloorSize * FloorScale) / 2.0f;
	for (int i = 0; i < FloorSpriteCount; i++)
	{
		Entity floor = GetWorld()->CreateEntity();
		floor.AddComponent<PhysicsBodyComponent>()->Dynamic = false;

		auto floorTransform = floor.AddComponent<TransformComponent>();
		floorTransform->Position = { initialX + i * FloorSize * FloorScale, floorY };
		floorTransform->Scale = { FloorScale, FloorScale };

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

		for(int j = 0; j < FloorSpriteDepth; j++)
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

		GAME_LOG_DEBUG("ADDED FLOOR SPRITE [" + to_string(floor.ID) + "]");
	}
}
