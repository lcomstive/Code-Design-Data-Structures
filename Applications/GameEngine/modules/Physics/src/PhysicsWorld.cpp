#include <ECS/world.hpp>
#include <Physics/PhysicsWorld.hpp>
#include <GameUtilities/messagebus.hpp>
#include <ECS/components/transform-component.hpp>

using namespace ECS;
using namespace Physics;
using namespace Utilities;

PhysicsWorld::PhysicsWorld(World* entityWorld, Vector2 gravity) :
	m_Objects(),
	m_Gravity(gravity),
	m_EntityWorld(entityWorld) { }

PhysicsWorld::~PhysicsWorld() = default;

void PhysicsWorld::Step(float deltaTime)
{
	for(auto& pair : m_Objects)
	{
		PhysicsObject& obj = pair.second;
		auto transform = m_EntityWorld->GetComponent<TransformComponent>(pair.first);

		Vector2 force = obj.Mass * m_Gravity;
		Vector2 acceleration = force / obj.Mass;

		obj.Velocity += acceleration * deltaTime;
		transform->Position += obj.Velocity * deltaTime;
	}
}

void PhysicsWorld::BroadphaseCollision()
{
	auto copy = m_Objects;
	for(auto& pair : copy)
	{
		for(auto& pair2 : copy)
		{
			if(pair.first == pair2.first)
				continue; // Same object

			if(pair.second.Collider.Intersects(pair2.second.Collider))
				MessageBus::eventBus()->Send("PhysicsCollision", DataStream().write(m_EntityWorld->ID())->write(pair.first)->write(pair2.first));
		}
	}
}

void PhysicsWorld::Remove(ECS::EntityID id)
{
	m_Objects.erase(id);
}

PhysicsObject* PhysicsWorld::Add(ECS::EntityID id)
{
	if(m_Objects.find(id) == m_Objects.end())
		m_Objects.emplace(id, PhysicsObject());
	return &m_Objects[id];
}

World* PhysicsWorld::GetEntityWorld() { return m_EntityWorld; }
unsigned int PhysicsWorld::GetObjectCount() { return (unsigned int)m_Objects.size(); }