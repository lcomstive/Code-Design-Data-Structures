#pragma once
#include <map>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <ECS/system.hpp>

using namespace std;

namespace ECS
{
	class SystemManager
	{
	public:
		SystemManager();

		void Initialize();
		void Destroy();

		template<typename T>
		T* Add()
		{
			if (!is_base_of<System, T>())
				return nullptr;

			T* system = new T();
			m_Systems.emplace(typeid(T), system);
			return system;
		}

		template<typename T>
		void Remove()
		{
			if (!hasSystem<T>())
				return;
			type_index type = typeid(T);
			delete m_Systems[type];
			m_Systems.erase(type);
		}

		template<typename T>
		bool Has() { return m_Systems.find(typeid(T)) != m_Systems.end(); }

		template<typename T>
		T* Get()
		{
			if (!hasSystem<T>())
				return nullptr;
			return m_Systems[typeid(T)];
		}

		void Draw();
		void Update(float deltaTime);

		vector<System*> all();

	private:
		map<type_index, System*> m_Systems;
	};
}