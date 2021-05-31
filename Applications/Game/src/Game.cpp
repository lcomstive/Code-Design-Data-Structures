#include <Game.hpp>
#include <iostream>
#include <raylib.h>

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
	SetExitKey(0); // Don't exit when pressing ESCAPE (default)

	if (args.VSync)
		SetTargetFPS(GetMonitorRefreshRate(0));

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

	cameraComponent->Zoom = 1.0f;

	// --- PLAYER --- //
	m_Player = m_World->CreateEntity();
	m_Player.AddComponent<PlayerInputComponent>();
	m_Player.AddComponent<DebugNameComponent>()->Name = "Player";

	auto playerTransform = m_Player.AddComponent<TransformComponent>();
	playerTransform->Scale.x = playerTransform->Scale.y = (resolution.x * dpi.x) / 200.0f;
	playerTransform->Position = { resolution.x * -0.3f, resolution.y * 0.15f, 99 };

	auto playerSprite = m_Player.AddComponent<AnimatedSpriteComponent>();
	/*
	playerSprite->Sprite = ResourceManager::LoadTexture("assets/Huntress/Sprites/Idle.png");
	playerSprite->Subdivide(8);
	*/
	playerSprite->Sprite = ResourceManager::LoadTexture(DinoTextureBase + DinoTextures[DinoTextureIndex] + ".png");
	Texture texture = ResourceManager::GetTexture(playerSprite->Sprite);
	playerSprite->MaxFrames = 4;
	playerSprite->ReferenceSize = { 0, 0, texture.width / 24.0f, (float)texture.height };
	
	m_Player.AddComponent<PhysicsBodyComponent>();

	m_InputSystem->Map(KEY_W,  "PlayerJump");
	m_InputSystem->Map(KEY_UP, "PlayerJump");
	m_InputSystem->Map(KEY_S,  "PlayerCrouch", InputBindingState::Held);
	m_InputSystem->Map(KEY_DOWN, "PlayerCrouch", InputBindingState::Held);
	m_InputSystem->Map(KEY_S,  "PlayerRunning", InputBindingState::Up);
	m_InputSystem->Map(KEY_DOWN, "PlayerRunning", InputBindingState::Up);

	m_InputSystem->Map(KEY_ESCAPE, "KeyQuit", [&](DataStream) { Quit(); });

	MessageBus::eventBus()->Send("PlayerRunning");

	/*
	m_InputSystem->Map(KEY_Q, "PlaySound1", [&](DataStream)
		{
			Entity e = m_World->CreateEntity();
			e.AddComponent<AudioComponent>()
				->Sound = ResourceManager::LoadSound("assets/Sounds/monke_1.wav");
		}, InputBindingState::Down);
	*/

	const int FloorSpriteCount = 9;
	float strideX = 14.0f;
	float scale = 7.5f;
	float initialX = resolution.x / -2.0f;
	for(int i = 0; i < FloorSpriteCount; i++)
	{
		Entity floor = m_World->CreateEntity();
		floor.AddComponent<PhysicsBodyComponent>()->Dynamic = false;

		auto floorTransform = floor.AddComponent<TransformComponent>();
		floorTransform->Position = { initialX + i * strideX * scale, resolution.y * 0.35f};
		floorTransform->Scale = { scale, scale };

		auto floorSprite = floor.AddComponent<AnimatedSpriteComponent>();
		floorSprite->Sprite = ResourceManager::LoadTexture("assets/kenney_pixelplatformer/Tilemap/tiles_packed.png");
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

		GAME_LOG_DEBUG("ADDED FLOOR SPRITE [" + to_string(floor.ID) + "]");
	}
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

	if(GuiButton({ 20, 60, 110, 30 }, "Idle")) MessageBus::eventBus()->Send("PlayerIdle");
	if(GuiButton({ 20, 100, 110, 30 }, "Run")) MessageBus::eventBus()->Send("PlayerRunning");
	if(GuiButton({ 20, 140, 110, 30 }, "Hurt")) MessageBus::eventBus()->Send("PlayerHurt");
	if(GuiButton({ 20, 180, 110, 30 }, "Crouch")) MessageBus::eventBus()->Send("PlayerCrouch");

	GuiLine({ 10, 250, 130, 30 }, "Dino Texture");
	GuiSpinner({ 10, 260, 130, 30 }, "", &DinoTextureIndex, 0, DinoTextures.size() - 1, false);
	DinoTextureIndex = clamp(DinoTextureIndex, 0, (int)DinoTextures.size() - 1);
	m_Player.GetComponent<AnimatedSpriteComponent>()->Sprite = ResourceManager::LoadTexture(DinoTextureBase + DinoTextures[DinoTextureIndex] + ".png");
}
