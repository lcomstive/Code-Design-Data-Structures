#include <ECS/world.hpp>
#include <GameUtilities/messagebus.hpp>
#include <GameEngine/Systems/PhysicsSystem.hpp>
#include <ECS/components/transform-component.hpp>
#include <GameEngine/Components/PhysicsBodyComponent.hpp>

using namespace std;
using namespace ECS;
using namespace Physics;
using namespace Utilities;

string PhysicsBodyComponentName = typeid(PhysicsBodyComponent).name();

void PhysicsSystem::Init()
{
	m_World = make_unique<PhysicsWorld>(GetWorld());

	m_ComponentRemoveEventID = MessageBus::eventBus()->AddReceiver("ComponentAdd" + PhysicsBodyComponentName, [&](DataStream stream)
	{
		if(stream.read<EntityID>() != GetWorld()->ID())
			return; // Not for this system
		EntityID entity = stream.read<EntityID>();

		PhysicsObject* body = m_World->Add(entity);

		auto transform = GetWorld()->GetComponent<TransformComponent>(entity);
		auto physicsBody = GetWorld()->GetComponent<PhysicsBodyComponent>(entity);

		body->Mass = physicsBody->Mass;
		body->Velocity = physicsBody->Force;
		body->Collider = AABB::FromCenter(transform->Position + physicsBody->Offset, transform->Scale * physicsBody->Scale);

		physicsBody->Force = { 0, 0 };
		
		m_Bodies.emplace(entity, body);
	});

	m_ComponentRemoveEventID = MessageBus::eventBus()->AddReceiver("ComponentRemove" + PhysicsBodyComponentName, [&](DataStream stream)
	{
		if(stream.read<EntityID>() != GetWorld()->ID())
			return; // Not for this system
		EntityID entity = stream.read<EntityID>();
		m_World->Remove(entity);
		m_Bodies.erase(entity);
	});
}

void PhysicsSystem::Destroy()
{
	MessageBus::eventBus()->RemoveReceiver("ComponentRemove" + PhysicsBodyComponentName, m_ComponentRemoveEventID);
}

void PhysicsSystem::Update(float deltaTime)
{
	for(auto& pair : m_Bodies)
	{
		auto pbody = GetWorld()->GetComponent<PhysicsBodyComponent>(pair.first);
		if(!pbody)
			return; // TODO: Delete from m_Bodies?
		auto transform = GetWorld()->GetComponent<TransformComponent>(pair.first);
		pair.second->Collider = AABB::FromCenter(transform->Position + pbody->Offset, transform->Scale * pbody->Scale);
	}

	m_World->Step(deltaTime);
}

void PhysicsSystem::Draw()
{
#ifndef NDEBUG
	if(!DrawDebugInfo)
		return;

	for(auto& pair : m_Bodies)
	{
		AABB collider = pair.second->Collider;
		DrawRectangleLines((int)collider.Min.x, (int)collider.Min.y, (int)collider.width(), (int)collider.height(), PINK);
	}
#endif
}


PhysicsWorld* PhysicsSystem::GetPhysicsWorld()  { return m_World.get(); }
PhysicsObject* PhysicsSystem::GetPhysicsObject(ECS::EntityID id)  { return m_Bodies[id]; }