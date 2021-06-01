#pragma once
#include <raylib.h>
#include <ECS/world.hpp>
#include <GameUtilities/log.hpp>
#include <GameUtilities/timer.hpp>
#include <Systems/InputSystem.hpp>

struct GameArgs
{
	uint32_t Width		= 800;
	uint32_t Height		= 600;
	bool Resizable		= true;
	
	bool VSync			= true;
	bool Fullscreen		= false;
	std::string Title	= "Game";
};

class Game
{
	bool m_Running;
	Utilities::Timer m_DeltaTimer;
	unique_ptr<ECS::World> m_World;
	InputSystem* m_InputSystem;

	ECS::Entity m_Player;
	ECS::Entity m_Camera;

public:
	Game(const GameArgs& args = {});
	~Game();

	void Run();
	void Quit();

	bool IsRunning() const;

	Vector2 GetResolution() const;

private:
	void DrawGUI();
	void Initialise();
	void Update(float deltaTime);

	void CreatePlayer();
	void CreateFloors();
};