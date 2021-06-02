#pragma once
#include <GameUtilities/messagebus.hpp>

namespace ECS
{
	class World;

	class System
	{
	private:
		World* m_World;
		void setWorld(World* worldHandle) { m_World = worldHandle; }

		friend class World;

	public:
		virtual void Init() { }
		virtual void Update(float deltaTime) { }
		virtual void Draw() { }
		virtual void Destroy() { }

		World* GetWorld() { return m_World; }
		Utilities::MessageBus* Events() { return Utilities::MessageBus::eventBus(); }
	};
}