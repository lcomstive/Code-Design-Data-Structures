#include <Game.hpp>
#include <iostream>
#include <raylib.h>
#include <GLFW/glfw3.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h> // Immediate-mode GUI
#include <rlgl.h>

// Components
#include <Components/AudioComponent.hpp>
#include <Components/CameraComponent.hpp>
#include <Components/SpriteComponent.hpp>
#include <Components/PlayerInputComponent.hpp>
#include <Components/PhysicsBodyComponent.hpp>
#include <Components/AnimatedSpriteComponent.hpp>

// Systems
#include <Systems/AudioSystem.hpp>
#include <Systems/PhysicsSystem.hpp>
#include <Systems/AnimationSystem.hpp>
#include <Systems/PlayerStateSystem.hpp>
#include <Systems/SpriteRenderSystem.hpp>

using namespace ECS;
using namespace Utilities;

const string DinoTextureBase = "assets/DinoCharacters/sheets/DinoSprites - ";
vector<string> DinoTextures =
{
	"doux",
	"mort",
	"tard",
	"vita"
};
int DinoTextureIndex = 0;

Game::Game(const GameArgs& args)
{
	PROFILE_BEGIN("startup.json");

	m_World = make_unique<World>();
	m_DeltaTimer = Timer("Delta Timer", false);

	// Init raylib window
	InitWindow(args.Width, args.Height, args.Title.c_str());

	unsigned int windowFlags = FLAG_WINDOW_HIGHDPI;
	if(args.VSync) windowFlags |= FLAG_VSYNC_HINT;
	if(args.Resizable) windowFlags |= FLAG_WINDOW_RESIZABLE;
	SetWindowState(windowFlags);
	SetExitKey(0); // Don't exit when pressing ESCAPE (default)

	Initialise();

	PROFILE_END(); // startup.json
}

Game::~Game() { Quit(); }

void Game::Initialise()
{
	Vector2 dpi = GetWindowScaleDPI();
	Vector2 resolution =
	{
			(float)GetScreenWidth(),
			(float)GetScreenHeight()
	};

	Log::Initialize();
	MessageBus::eventBus()->AddReceiver("LogError", [](DataStream stream) { cerr << "[ERROR] " << stream.read<string>(); });

	rlViewport(0, 0, resolution.x * dpi.x, resolution.y * dpi.y);

	// --- SYSTEMS --- //
	m_World->AddSystem<AudioSystem>();
	m_World->AddSystem<AnimationSystem>();
	m_World->AddSystem<PlayerStateSystem>();
	m_World->AddSystem<SpriteRenderSystem>();
	m_InputSystem = m_World->AddSystem<InputSystem>();

	m_World->Initialize();

	// --- CAMERA --- //
	m_Camera = m_World->CreateEntity();
	m_Camera.AddComponent<TransformComponent>()->Position = { 0, 0, 0 };
	CameraComponent* cameraComponent = m_Camera.AddComponent<CameraComponent>();

	cameraComponent->Zoom = 1;

	// --- INPUT --- //
	m_InputSystem->Map(KEY_W, "PlayerJump");
	m_InputSystem->Map(KEY_UP, "PlayerJump");
	m_InputSystem->Map(KEY_S, "PlayerCrouch", InputBindingState::Held);
	m_InputSystem->Map(KEY_DOWN, "PlayerCrouch", InputBindingState::Held);
	m_InputSystem->Map(KEY_S, "PlayerRunning", InputBindingState::Up);
	m_InputSystem->Map(KEY_DOWN, "PlayerRunning", InputBindingState::Up);

	m_InputSystem->Map(KEY_ESCAPE, "KeyQuit", [&](DataStream) { Quit(); });

	// Set default state
	MessageBus::eventBus()->Send("PlayerRunning");

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
}

void Game::Run()
{
	PROFILE_BEGIN("runtime.json");
	m_Running = true;
	m_DeltaTimer.Start();
	while (m_Running && !WindowShouldClose())
	{
		m_DeltaTimer.Stop();
		auto deltaTime = m_DeltaTimer.elapsedTimeMS();
		m_DeltaTimer.Start();

		Update(deltaTime.count() / 1000.0f);

		DrawFPS(10, 10);
	}

	PROFILE_END(); // runtime.json

	Quit();
	CloseWindow();
}

void Game::Update(float deltaTime)
{
	// If window resizes, alter camera viewport to match
	Vector2 dpi = GetWindowScaleDPI();
	m_Camera.GetComponent<CameraComponent>()->Viewport = { 0, 0, GetScreenWidth() * dpi.x, GetScreenHeight() * dpi.y };

	BeginDrawing();
	ClearBackground({ 10, 10, 10, 255 });

	m_World->Update(deltaTime);
	DrawGUI();

	EndDrawing();
}

void Game::Quit()
{
	if (!IsRunning())
		return;
	m_Running = false;
}

bool Game::IsRunning() const { return m_Running; }

void Game::DrawGUI()
{
	GuiGroupBox({ 10, 50, 130, 180 }, "Dino State");

	if (GuiButton({ 20, 60, 110, 30 }, "Idle")) MessageBus::eventBus()->Send("PlayerIdle");
	if (GuiButton({ 20, 100, 110, 30 }, "Run")) MessageBus::eventBus()->Send("PlayerRunning");
	if (GuiButton({ 20, 140, 110, 30 }, "Hurt")) MessageBus::eventBus()->Send("PlayerHurt");
	if (GuiButton({ 20, 180, 110, 30 }, "Crouch")) MessageBus::eventBus()->Send("PlayerCrouch");

	GuiLine({ 10, 250, 130, 30 }, "Dino Texture");
	GuiSpinner({ 10, 260, 130, 30 }, "", &DinoTextureIndex, 0, DinoTextures.size() - 1, false);
	DinoTextureIndex = clamp(DinoTextureIndex, 0, (int)DinoTextures.size() - 1);
	m_Player.GetComponent<AnimatedSpriteComponent>()->Sprite = ResourceManager::LoadTexture(DinoTextureBase + DinoTextures[DinoTextureIndex] + ".png");
}

const float PlayerSize = 7.5f;
const float FloorScale = 8.0f;
const float FloorSize = 14.0f; // pixels

void Game::CreatePlayer()
{
	Vector2 resolution = GetResolution();
	Vector2 dpi = GetWindowScaleDPI();

	// --- PLAYER --- //
	m_Player = m_World->CreateEntity();
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
	Vector2 resolution = GetResolution();

	static const int FloorSpriteCount = 20;
	static const int FloorSpriteDepth = 10;
	float initialX = 0;
	float floorY = resolution.y - (FloorSize * FloorScale) / 2.0f;
	for (int i = 0; i < FloorSpriteCount; i++)
	{
		Entity floor = m_World->CreateEntity();
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
			Entity depth = m_World->CreateEntity();
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

Vector2 Game::GetResolution() const { return Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() }; }
