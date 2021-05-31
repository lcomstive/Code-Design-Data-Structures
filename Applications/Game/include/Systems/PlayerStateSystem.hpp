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

private:
	unsigned int m_IdleEventID;
	unsigned int m_HurtEventID;
	unsigned int m_CrouchEventID;
	unsigned int m_RunningEventID;
	
	PlayerState m_State = PlayerState::Idle;
	PlayerState m_PreviousState = PlayerState::Crouching;
};