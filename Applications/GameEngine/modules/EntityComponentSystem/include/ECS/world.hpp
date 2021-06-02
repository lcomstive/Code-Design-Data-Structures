#pragma once
#include <unordered_map>
#include <ECS/entity-manager.hpp>
#include <ECS/system-manager.hpp>
#include <ECS/component-manager.hpp>

namespace ECS
{
	class World
	{
		unsigned int m_ID;
		unique_ptr<EntityManager> m_EntityManager;
		unique_ptr<SystemManager> m_SystemManager;
		unique_ptr<ComponentManager> m_ComponentManager;

		static unordered_map<unsigned int, World*> s_Worlds;

	public:
		struct Entity
		{
			EntityID ID;
			World* world;

			Entity(ECS::EntityID entity = 0, World* worldHandle = nullptr) : ID(entity), world(worldHandle) { }

			void Destroy() { world->DestroyEntity(ID); }

			template<typename T>
			T* AddComponent() { return world ? world->AddComponent<T>(ID) : nullptr; }

			template<typename T1, typename T2>
			void AddComponent() { if(world)world->AddComponent<T1, T2>(ID); }

			template<typename T1, typename T2, typename T3>
			void AddComponent() { if(world) world->AddComponent<T1, T2, T3>(ID); }

			template<typename T1, typename T2, typename T3, typename T4>
			void AddComponent() { if(world) world->AddComponent<T1, T2, T3>(ID); }

			template<typename T>
			T* GetComponent() { return world ? world->GetComponent<T>(ID) : nullptr; }

			template<typename T>
			void RemoveComponent() { if(world) world->RemoveComponent<T>(ID); }

			template<typename T>
			bool HasComponent() { return world && world->HasComponent<T>(ID); }

			bool HasComponents() { return world && world->HasComponents(ID); }

			template<typename T1, typename T2>
			bool HasComponents() { return world && world->HasComponents<T1, T2>(ID); }

			template<typename T1, typename T2, typename T3>
			bool HasComponents() { return world && world->HasComponents<T1, T2, T3>(ID); }

			template<typename T1, typename T2, typename T3, typename T4>
			bool HasComponents() { return world && world->HasComponents<T1, T2, T3, T4>(ID); }

			void ClearComponents() { if(world) world->ClearComponents(ID); }

			bool isValid() { return world != nullptr; }

			bool operator ==(EntityID id) { return id == ID; }
			bool operator !=(EntityID id) { return id != ID; }
		};

		World();

		void Destroy();
		void Initialize();
		void Update(float deltaTime);
		void Draw();

		/// ENTITY ///
		void PrepareEntities(unsigned int count);
		Entity CreateEntity();
		Entity GetEntity(EntityID entity);
		void DestroyEntity(EntityID entity);

		template<typename T>
		Entity CreateEntity()
		{
			Entity e = Entity(m_EntityManager->Create<T>(), this);
			m_Entities.emplace(e.ID, e);
			return e;
		}

		template<typename T1, typename T2>
		Entity CreateEntity()
		{
			Entity e = Entity(m_EntityManager->Create<T1, T2>(), this);
			m_Entities.emplace(e.ID, e);
			return e;
		}

		template<typename T1, typename T2, typename T3>
		Entity CreateEntity()
		{
			Entity e = Entity(m_EntityManager->Create<T1, T2, T3>(), this);
			m_Entities.emplace(e.ID, e);
			return e;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		Entity CreateEntity()
		{
			Entity e = Entity(m_EntityManager->Create<T1, T2, T3, T4>(), this);
			m_Entities.emplace(e.ID, e);
			return e;
		}

		unsigned int GetEntityCount() { return m_EntityManager->entityCount(); }

		vector<Entity> GetEntities()
		{
			vector<EntityID> IDs = m_EntityManager->entities();
			vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		template<typename T>
		vector<Entity> GetEntities()
		{
			vector<EntityID> IDs = m_EntityManager->entities<T>();
			vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		template<typename T1, typename T2>
		vector<Entity> GetEntities()
		{
			vector<EntityID> IDs = m_EntityManager->entities<T1, T2>();
			vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		template<typename T1, typename T2, typename T3>
		vector<Entity> GetEntities()
		{
			vector<EntityID> IDs = m_EntityManager->entities<T1, T2, T3>();
			vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		vector<Entity> GetEntities()
		{
			vector<EntityID> IDs = m_EntityManager->entities<T1, T2, T3, T4>();
			vector<Entity> entities(IDs.size());
			for (size_t i = 0; i < IDs.size(); i++)
				entities[i] = GetEntity(IDs[i]);
			return entities;
		}

		/// COMPONENT ///
		template<typename T>
		T* GetComponent(EntityID entity) { return m_ComponentManager->Get<T>(entity); }

		template<typename T>
		unordered_map<EntityID, T*> GetComponents() { return m_ComponentManager->Get<T>(); }

		template<typename T1, typename T2>
		unordered_map<EntityID, pair<T1*, T2*>> GetComponents() { return m_ComponentManager->Get<T1, T2>(); }

		template<typename T>
		T* AddComponent(EntityID entity) { return m_ComponentManager->Add<T>(entity); }

		template<typename T1, typename T2>
		void AddComponent(EntityID entity) { m_ComponentManager->Add<T1, T2>(entity); }

		template<typename T1, typename T2, typename T3>
		void AddComponent(EntityID entity) { m_ComponentManager->Add<T1, T2, T3>(entity); }

		template<typename T1, typename T2, typename T3, typename T4>
		void AddComponent(EntityID entity) { m_ComponentManager->Add<T1, T2, T3, T4>(entity); }

		template<typename T>
		void RemoveComponent(EntityID entity) { m_ComponentManager->Remove<T>(entity); }

		template<typename T>
		bool HasComponent(EntityID entity) { return m_ComponentManager->Has<T>(entity); }

		bool HasComponents(EntityID entity) { return !m_ComponentManager->Empty(entity); }

		template<typename T1, typename T2>
		bool HasComponents(EntityID entity) { return m_ComponentManager->Has<T1, T2>(entity); }

		template<typename T1, typename T2, typename T3>
		bool HasComponents(EntityID entity) { return m_ComponentManager->Has<T1, T2, T3>(entity); }

		template<typename T1, typename T2, typename T3, typename T4>
		bool HasComponents(EntityID entity) { return m_ComponentManager->Has<T1, T2, T3, T4>(entity); }

		void ClearComponents(EntityID entity);

		/// SYSTEM ///
		template<typename T>
		T* AddSystem()
		{
			T* system = m_SystemManager->Add<T>();
			((System*)system)->setWorld(this);
			return system;
		}

		template<typename T>
		void RemoveSystem() { m_SystemManager->Remove<T>(); }

		template<typename T>
		bool HasSystem() { return m_SystemManager->Has<T>(); }

		template<typename T>
		T* GetSystem() { return m_SystemManager->Get<T>(); }

		/// Getters ///
		EntityID ID();
		EntityManager* GetEntityManager();
		SystemManager* GetSystemManager();
		ComponentManager* GetComponentManager();

		static World* Global() { return s_Worlds.size() > 0 ? s_Worlds[0] : nullptr; }
		static World* GetWorld(unsigned int id) { return s_Worlds.find(id) == s_Worlds.end() ? nullptr : s_Worlds[id]; }

	private:
		unordered_map<EntityID, Entity> m_Entities;
	};

	typedef struct World::Entity Entity;
}

inline bool operator ==(const ECS::EntityID& id, const ECS::Entity& e) { return id == e.ID; }
inline bool operator !=(const ECS::EntityID& id, const ECS::Entity& e) { return id != e.ID; }
