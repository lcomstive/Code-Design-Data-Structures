#pragma once
#include <GameEngine/Application.hpp>

class Game : public Application
{
	ECS::Entity m_Player;

public:
	Game(const ApplicationArgs& args = { });

protected:
	void OnDrawGUI() override;
	void OnUpdate(float deltaTime) override;

private:
	void CreatePlayer();
	void CreateFloors();
};