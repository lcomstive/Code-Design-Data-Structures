#pragma once
#include <vector>
#include <ECS/system.hpp>

class PlayerStateSystem : public ECS::System
{
public:
	enum class PlayerState : char { Idle, Running, Hurt, Crouching };

	void Init() override;
	void Destroy() override;
	void Update(float deltaTime) override;

	PlayerState GetCurrentState();

private:
	unsigned int m_IdleEventID = 0;
	unsigned int m_HurtEventID = 0;
	unsigned int m_CrouchEventID = 0;
	unsigned int m_RunningEventID = 0;
	
	PlayerState m_State = PlayerState::Idle;
	PlayerState m_PreviousState = PlayerState::Crouching;
};