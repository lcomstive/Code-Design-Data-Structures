#pragma once
#include <map>
#include <ECS/system.hpp>

struct PhysicsBodyData; // Forward declaration. Defined in physac.h

class PhysicsSystem : public ECS::System
{
public:
	bool DrawDebugInfo = false;
	
	void Init() override;
	void Destroy() override;

	void Draw() override;
	void Update(float deltaTime) override;

private:
	unsigned int m_ComponentRemoveEventID;

	map<ECS::EntityID, PhysicsBodyData*> m_Bodies;
};