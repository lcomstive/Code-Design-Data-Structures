#pragma once
#include <raylib.h>
#include <ECS/world.hpp>
#include <GameUtilities/log.hpp>
#include <GameUtilities/timer.hpp>
#include <GameUtilities/messagebus.hpp>
#include <GameEngine/Systems/InputSystem.hpp>

struct ApplicationArgs
{
	uint32_t Width		= 800;
	uint32_t Height		= 600;
	bool Resizable		= true;
	
	bool VSync			= false;
	bool Fullscreen		= false;
	std::string Title	= "Game";
};

class Application
{
	bool m_Running;
	unique_ptr<ECS::World> m_World;
	InputSystem* m_InputSystem;

	ECS::Entity m_Camera;

public:
	Application(const ApplicationArgs& args = {});
	~Application();

	void Run();
	void Quit();

	// --- GETTERS --- //
	bool IsRunning() const;

	Vector2 GetResolution() const;
	Vector2 GetResolutionDPI() const;
	InputSystem* GetInput() const;

	ECS::Entity& MainCamera();
	ECS::World* GetWorld();

	Utilities::MessageBus* Events() const;

protected:
	virtual void OnDestroy();
	virtual void OnDrawGUI();
	virtual void OnUpdate(float deltaTime);

private:
	void Initialise();
	void Update(float deltaTime);
};

#define APPLICATION_MAIN(gameType) int main() { \
										PROFILE_BEGIN("startup.json"); \
										gameType game; \
										PROFILE_END(); \
										PROFILE_BEGIN("runtime.json"); \
										game.Run(); \
										PROFILE_END(); \
										return 0; }