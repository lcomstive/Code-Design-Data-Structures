#include <ECS/world.hpp>
#include <Systems/PhysicsSystem.hpp>
#include <GameUtilities/messagebus.hpp>
#include <Components/PhysicsBodyComponent.hpp>

using namespace std;
using namespace ECS;
using namespace Utilities;

string PhysicsBodyComponentName = typeid(PhysicsBodyComponent).name();

void PhysicsSystem::Init()
{
	// InitPhysics();

	m_ComponentRemoveEventID = MessageBus::eventBus()->AddReceiver("ComponentRemove" + PhysicsBodyComponentName, [&](DataStream stream)
	{
		if(stream.read<EntityID>() != world()->ID())
			return; // Not for this system
		EntityID entity = stream.read<EntityID>();
		if(m_Bodies.find(entity) == m_Bodies.end())
			return;

		// DestroyPhysicsBody(m_Bodies[entity]);
		m_Bodies.erase(entity);
	});
}

void PhysicsSystem::Destroy()
{
	MessageBus::eventBus()->RemoveReceiver("ComponentRemove" + PhysicsBodyComponentName, m_ComponentRemoveEventID);

	// ClosePhysics();
}

void PhysicsSystem::Update(float deltaTime)
{
	auto pairs = world()->GetComponents<TransformComponent, PhysicsBodyComponent>();

	for(auto& pair : pairs)
	{
		TransformComponent* transform = pair.second.first;

		/*
		if(m_Bodies.find(pair.first) == m_Bodies.end())
			m_Bodies.emplace(pair.first,CreatePhysicsBodyRectangle({ 0, 0 }, transform->Scale.x, transform->Scale.y, 1.0f));

		m_Bodies[pair.first]->force = pair.second.second->Force;
		m_Bodies[pair.first]->enabled = pair.second.second->Dynamic;
		pair.second.second->Force = { 0, 0 };
		*/
	}
}

void PhysicsSystem::Draw()
{
#ifndef NDEBUG
	if(!DrawDebugInfo)
		return;

	/*
	for(auto& pair : m_Bodies)
	{
		PhysicsBody body = pair.second;
		int vertexCount = body->shape.vertexData.vertexCount;
        for (int j = 0; j < vertexCount; j++)
        {
            // Get physics bodies shape vertices to draw lines
            // Note: GetPhysicsShapeVertex() already calculates rotation transformations
            Vector2 vertexA = GetPhysicsShapeVertex(body, j);

            int jj = (((j + 1) < vertexCount) ? (j + 1) : 0);   // Get next vertex or first to close the shape
            Vector2 vertexB = GetPhysicsShapeVertex(body, jj);

            DrawLineV(vertexA, vertexB, GREEN);     // Draw a line between two vertex positions
        }
	}
	*/
#endif
}
