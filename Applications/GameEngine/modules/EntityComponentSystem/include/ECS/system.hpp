#pragma once
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

		World* world() { return m_World; }
	};
}