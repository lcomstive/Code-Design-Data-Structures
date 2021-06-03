#pragma once
#include <map>
#include <GameUtilities/math.hpp>
#include <Physics/PhysicsObject.hpp>

namespace Physics
{
	class PhysicsWorld
	{
		ECS::World* m_EntityWorld;
		Vector2 m_Gravity;

		std::unordered_map<ECS::EntityID, PhysicsObject> m_Objects;
		std::unordered_map<ECS::EntityID, bool> m_CollisionLastFrame;

		void BroadphaseCollision();

	public:
		PhysicsWorld(ECS::World* entityWorld, Vector2 gravity = { 0, 0 });
		~PhysicsWorld();

		void Step(float deltaTime);

		ECS::World* GetEntityWorld();
		unsigned int GetObjectCount();

		Vector2& GetGravity();
		void SetGravity(Vector2& v);

		void Remove(ECS::EntityID id);
		PhysicsObject* Add(ECS::EntityID id);
	};
}