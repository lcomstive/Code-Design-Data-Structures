#include <Game.hpp>
#include <iostream>
#include <raylib.h>
#include <Systems/AudioSystem.hpp>
#include <Systems/AnimationSystem.hpp>
#include <Systems/SpriteRenderSystem.hpp>
#include <Components/SpriteComponent.hpp>
#include <Components/AnimatedSpriteComponent.hpp>

using namespace ECS;
using namespace Utilities;

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

void MovePlayerTest(Entity player, Vector2 direction)
{
	Vector3* position = &player.GetComponent<TransformComponent>()->Position;
	position->x += direction.x;
	position->y += direction.y;
}

void Game::Initialise()
{
	Log::Initialize();
	MessageBus::eventBus()->AddReceiver("LogError", [](DataStream stream) { cerr << "[ERROR] " << stream.read<string>(); });
	
	// --- SYSTEMS --- //
	m_World->AddSystem<AudioSystem>();
	m_World->AddSystem<AnimationSystem>();
	m_World->AddSystem<SpriteRenderSystem>();
	m_InputSystem = m_World->AddSystem<InputSystem>();

	m_World->Initialize();

	// --- PLAYER --- //
	m_Player = m_World->CreateEntity();
	m_Player.AddComponent<DebugNameComponent>()->Name = "Player";

	auto playerTransform = m_Player.AddComponent<TransformComponent>();
	playerTransform->Position =
	{
		GetScreenWidth() / 2.0f,
		GetScreenHeight() / 2.0f
	};
	playerTransform->Scale = { 1.5f, 1.5f };

	auto playerSprite = m_Player.AddComponent<AnimatedSpriteComponent>();
	playerSprite->Sprite = ResourceManager::LoadTexture("assets/Huntress/Sprites/Idle.png");
	playerSprite->Subdivide(8);

	m_InputSystem->Map(KEY_W, "PlayerMoveUp",    [&](DataStream) { MovePlayerTest(m_Player, {  0,  1 }); });
	m_InputSystem->Map(KEY_S, "PlayerMoveDown",  [&](DataStream) { MovePlayerTest(m_Player, {  0, -1 }); });
	m_InputSystem->Map(KEY_A, "PlayerMoveLeft",  [&](DataStream) { MovePlayerTest(m_Player, { -1,  0 }); });
	m_InputSystem->Map(KEY_D, "PlayerMoveRight", [&](DataStream) { MovePlayerTest(m_Player, {  1,  0 }); });

	m_InputSystem->Map(KEY_ESCAPE, "KeyQuit", [&](DataStream) { Quit(); });

	m_InputSystem->Map(KEY_Q, "PlaySound1", [&](DataStream)
	{
		Entity e = m_World->CreateEntity();
		e.AddComponent<AudioComponent>()
			->Sound = ResourceManager::LoadSound("assets/Sounds/monke_1.wav");
	}, InputBindingState::Down);
	m_InputSystem->Map(KEY_E, "PlaySound2", [&](DataStream)
	{
		Entity e = m_World->CreateEntity();
		e.AddComponent<AudioComponent>()
			->Sound = ResourceManager::LoadSound("assets/Sounds/monke_2.wav");
	}, InputBindingState::Down);
	m_InputSystem->Map(KEY_SPACE, "PlaySound3", [&](DataStream)
	{
		Entity e = m_World->CreateEntity();
		e.AddComponent<AudioComponent>()
			->Sound = ResourceManager::LoadSound("assets/Sounds/Marcus_Poggers.wav");
	}, InputBindingState::Down);
}

void Game::Run()
{
	PROFILE_BEGIN("runtime.json");
	m_DeltaTimer.Start();
	while (IsRunning() && !WindowShouldClose())
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
	ClearBackground({ 20, 20, 20, 255 });

	m_World->Update(deltaTime);

	EndDrawing();
}

void Game::Quit()
{
	if (!IsRunning())
		return;
	m_Running = false;
}

bool Game::IsRunning() const { return m_Running; }
