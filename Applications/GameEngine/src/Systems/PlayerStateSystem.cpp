#include <map>
#include <ECS/world.hpp>
#include <GameUtilities/messagebus.hpp>
#include <GameEngine/Systems/PlayerStateSystem.hpp>
#include <GameEngine/Components/PlayerInputComponent.hpp>
#include <GameEngine/Components/AnimatedSpriteComponent.hpp>

using namespace std;
using namespace ECS;
using namespace Utilities;

struct PlayerStateInfo
{
	unsigned int StartFrame = 0;
	unsigned int FrameCount = 1;
};

map<PlayerStateSystem::PlayerState, PlayerStateInfo> StateInfo =
{
	{ PlayerStateSystem::PlayerState::Idle, { 0, 3 } },
	{ PlayerStateSystem::PlayerState::Running, { 4, 6 } },
	{ PlayerStateSystem::PlayerState::Hurt, { 14, 3 } },
	{ PlayerStateSystem::PlayerState::Crouching, { 18, 6 } }
};

const string PlayerInputComponentName = typeid(PlayerInputComponent).name();

void PlayerStateSystem::Init()
{
	m_IdleEventID = MessageBus::eventBus()->AddReceiver("PlayerIdle", [&](DataStream) { m_State = PlayerState::Idle; });
	m_HurtEventID = MessageBus::eventBus()->AddReceiver("PlayerHurt", [&](DataStream) { m_State = PlayerState::Hurt; });
	m_CrouchEventID = MessageBus::eventBus()->AddReceiver("PlayerCrouch", [&](DataStream) { m_State = PlayerState::Crouching; });
	m_RunningEventID = MessageBus::eventBus()->AddReceiver("PlayerRunning", [&](DataStream) { m_State = PlayerState::Running; });

	m_State = m_PreviousState = PlayerState::Idle;

	GAME_LOG_DEBUG("PlayerStateSystem ready");
}

void PlayerStateSystem::Destroy()
{
	MessageBus::eventBus()->RemoveReceiver("PlayerIdle", m_IdleEventID);
	MessageBus::eventBus()->RemoveReceiver("PlayerHurt", m_HurtEventID);
	MessageBus::eventBus()->RemoveReceiver("PlayerCrouch", m_CrouchEventID);
	MessageBus::eventBus()->RemoveReceiver("PlayerRunning", m_RunningEventID);
}

void PlayerStateSystem::Update(float deltaTime)
{
	if (m_State == m_PreviousState)
		return;

	m_PreviousState = m_State;
	PlayerStateInfo stateInfo = StateInfo[m_State];

	auto pairs = world()->GetComponents<AnimatedSpriteComponent, PlayerInputComponent>();
	for (auto& pair : pairs)
	{
		auto sprite = pair.second.first;
		Texture texture = ResourceManager::GetTexture(sprite->Sprite);

		sprite->Frame = 0;
		sprite->MaxFrames = stateInfo.FrameCount;
		sprite->ReferenceSize.x = sprite->ReferenceSize.width * stateInfo.StartFrame;
	}
}
