#pragma once
#include <map>
#include <vector>
#include <ECS/entity.hpp>
#include <ECS/component-manager.hpp>

using namespace std;

namespace ECS
{
	class EntityManager
	{
		unsigned int m_WorldID;
		ComponentManager* m_ComponentManager;

		static const unsigned int m_DefaultChunkSize = 512;
		bool* m_Entities;
		unsigned int m_EntityCount = 0;
		unsigned int m_AvailableEntityCount = 0;

		void AddChunk(unsigned int chunkSize = m_DefaultChunkSize);
		void RemoveChunk();
		EntityID NextAvailableEntityID();
	
	public:
		EntityManager(ComponentManager* componentManager, unsigned int worldID);

		void Destroy();

		EntityID Create();
		void Prepare(unsigned int count = m_DefaultChunkSize);

		template<typename T>
		EntityID Create()
		{
			PROFILE_FN();

			EntityID id = Create();
			m_ComponentManager->Add<T>(id);
			return id;
		}

		template<typename T1, typename T2>
		EntityID Create()
		{
			PROFILE_FN();

			EntityID id = Create();
			m_ComponentManager->Add<T1, T2>(id);
			return id;
		}

		template<typename T1, typename T2, typename T3>
		EntityID Create()
		{
			PROFILE_FN();

			EntityID id = Create();
			m_ComponentManager->Add<T1, T2, T3>(id);
			return id;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		EntityID Create()
		{
			PROFILE_FN();

			EntityID id = Create();
			m_ComponentManager->Add<T1, T2, T3, T4>(id);
			return id;
		}

		void Destroy(EntityID id);

		unsigned int entityCount() { return m_EntityCount - m_AvailableEntityCount; }

		vector<EntityID> entities() const;

		template<typename T>
		vector<EntityID> entities()
		{
			PROFILE_FN();
			return m_ComponentManager->GetEntities<T>();
		}

		template<typename T1, typename T2>
		vector<EntityID> entities()
		{
			PROFILE_FN();

			vector<EntityID> entities;
			for (EntityID i = 0; i < m_EntityCount; i++)
			{
				if (m_Entities[i]) // if entity available (not being used)
					continue;
				if (m_ComponentManager->Has<T1, T2>(i))
					entities.emplace_back(i);
			}
			return entities;
		}

		template<typename T1, typename T2, typename T3>
		vector<EntityID> entities()
		{
			PROFILE_FN();

			vector<EntityID> entities;
			for (EntityID i = 0; i < m_EntityCount; i++)
			{
				if (m_Entities[i]) // if entity available (not being used)
					continue;
				if (m_ComponentManager->Has<T1, T2, T3>(i))
					entities.emplace_back(i);
			}
			return entities;
		}

		template<typename T1, typename T2, typename T3, typename T4>
		vector<EntityID> entities()
		{
			PROFILE_FN();

			vector<EntityID> entities;
			for (EntityID i = 0; i < m_EntityCount; i++)
			{
				if (m_Entities[i]) // if entity available (not being used)
					continue;
				if (m_ComponentManager->Has<T1, T2, T3, T4>(i))
					entities.emplace_back(i);
			}
			return entities;
		}
	};
}