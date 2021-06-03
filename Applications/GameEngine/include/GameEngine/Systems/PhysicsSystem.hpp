#pragma once
#include <map>
#include <ECS/system.hpp>
#include <Physics/PhysicsWorld.hpp>

class PhysicsSystem : public ECS::System
{
public:
	bool DrawDebugInfo = false;
	
	void Init() override;
	void Destroy() override;

	void Draw() override;
	void Update(float deltaTime) override;

	Physics::PhysicsWorld* GetPhysicsWorld();
	Physics::PhysicsObject* GetPhysicsObject(ECS::EntityID id);

	Vector2& GetGravity();
	void SetGravity(Vector2 gravity);
	void SetGravity(Vector2& gravity);

private:
	unsigned int m_ComponentRemoveEventID;

	unique_ptr<Physics::PhysicsWorld> m_World;
	map<ECS::EntityID, Physics::PhysicsObject*> m_Bodies;
};