#include <rlgl.h>
#include <raylib.h>
#include <iostream>
#include <GameEngine/Application.hpp>
#include <GameEngine/CommonSystems.hpp>
#include <GameEngine/CommonComponents.hpp>

using namespace ECS;
using namespace Utilities;

Application::Application(const ApplicationArgs& args)
{
	m_World = make_unique<World>();

	// Init raylib window
	InitWindow(args.Width, args.Height, args.Title.c_str());

	unsigned int windowFlags = FLAG_WINDOW_HIGHDPI;
	if(args.VSync)	   windowFlags |= FLAG_VSYNC_HINT;
	if(args.Resizable) windowFlags |= FLAG_WINDOW_RESIZABLE;
	SetWindowState(windowFlags);
	SetExitKey(0); // Don't exit when pressing ESCAPE (default)

	Initialise();
}

Application::~Application() { Quit(); }

void Application::Initialise()
{
	Vector2 resolution = GetResolution();

	Log::Initialize();
	Events()->AddReceiver("LogError", [](DataStream stream) { cerr << "[ERROR] " << stream.read<string>(); });

	rlViewport(0, 0, (int)resolution.x, (int)resolution.y);

	// --- SYSTEMS --- //
	m_World->AddSystem<AudioSystem>();
	m_World->AddSystem<AnimationSystem>();
	m_World->AddSystem<PlayerStateSystem>();
	m_World->AddSystem<SpriteRenderSystem>();
	m_InputSystem = m_World->AddSystem<InputSystem>();

	m_World->Initialize();

	// --- CAMERA --- //
	m_Camera = m_World->CreateEntity();
	m_Camera.AddComponent<CameraComponent>();
	m_Camera.AddComponent<TransformComponent>();

	GAME_LOG_DEBUG("Game Engine initialised");
}

void Application::Run()
{
	m_Running = true;
	while (m_Running && !WindowShouldClose())
	{
		Update(GetFrameTime());

#ifndef NDEBUG
		DrawFPS(10, 10);
#endif
	}

	PROFILE_END(); // runtime.json

	PROFILE_BEGIN("shutdown.json");
	Quit();
	CloseWindow();
}

void Application::Update(float deltaTime)
{
	PROFILE_FN();

	// Ensure main camera viewport is same as window size
	// TODO: Find window resize callback to use instead of setting every frame
	Vector2 resolution = GetResolution();
	m_Camera.GetComponent<CameraComponent>()->Viewport = { 0, 0, resolution.x, resolution.y };

	BeginDrawing();
	ClearBackground(BLACK);

	OnUpdate(deltaTime);

	m_World->Update(deltaTime);
	m_World->Draw();

	OnDrawGUI();

	EndDrawing();
}

void Application::Quit()
{
	if (!IsRunning())
		return;
	m_Running = false;
	OnDestroy();
}

// --- VIRTUALS --- //
void Application::OnDestroy() { }

void Application::OnUpdate(float deltaTime) { }
void Application::OnDrawGUI() { }

// --- GETTERS --- //
bool Application::IsRunning() const { return m_Running; }

ECS::Entity& Application::MainCamera() { return m_Camera; }

Vector2 Application::GetResolution() const
{
	Vector2 dpi = GetWindowScaleDPI();
	return Vector2{ GetScreenWidth() * dpi.x, GetScreenHeight() * dpi.y };
}

InputSystem* Application::GetInput() const { return m_InputSystem; }
World* Application::GetWorld() const { return m_World.get(); }
MessageBus* Application::Events() const { return MessageBus::eventBus(); }