#pragma once
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <ECS/entity.hpp>
#include <GameUtilities/inline-profiler.hpp>

// Include common components for easier access //
#include <ECS/components/transform-component.hpp>
#include <ECS/components/debug-name-component.hpp>

#define MAD_COMPONENT_ARRAY_DEFAULT_ARRAY_SIZE 512

using namespace std;

namespace ECS
{
	class ComponentManager
	{
	private:
		struct ComponentData
		{
			vector<void*> Instances;
			unordered_map<EntityID, unsigned int> EntityIndex;

			ComponentData() { }

			void Destroy()
			{
				for (size_t i = 0; i < Instances.size(); i++)
					free(Instances[i]); // delete Instances[i];
				Instances.clear();
				EntityIndex.clear();
			}

			void Add(void* component, EntityID entity)
			{
				Instances.emplace_back(component);
				EntityIndex.emplace(entity, (unsigned int)Instances.size() - 1);
			}

			template<typename T>
			T* Get(EntityID entity) { return (T*)(Has(entity) ? Instances[EntityIndex[entity]] : nullptr); }

			template<typename T>
			unordered_map<EntityID, T*> Get()
			{
				unordered_map<EntityID, T*> map;
				map.reserve(Instances.size());
				for(const auto& pair : EntityIndex)
					map.emplace(pair.first, (T*)Instances[pair.second]);
				return map;
			}

			bool Has(EntityID entity) { return EntityIndex.find(entity) != EntityIndex.end(); }

			void Remove(EntityID entity)
			{
				unsigned int instanceIndex = EntityIndex[entity];
				free(Instances[instanceIndex]); // delete Instances[instanceIndex];
				Instances.erase(Instances.begin() + instanceIndex);

				EntityIndex.erase(entity);

				// Shift all indexes down
				for (const auto& pair : EntityIndex)
				{
					if (pair.second >= instanceIndex)
						EntityIndex[pair.first]--;
				}
			}

			vector<EntityID> entities()
			{
				vector<EntityID> entities;
				entities.resize(EntityIndex.size());
				unsigned int i = 0;
				for (const auto& pair : EntityIndex)
					entities[i++] = pair.first;
				return entities;
			}
		};

		unsigned int m_WorldID;

		unordered_map<type_index, ComponentData> m_ComponentArrays;
		unordered_map<EntityID, vector<type_index>> m_EntityComponents;

	public:
		ComponentManager(unsigned int worldID) : m_WorldID(worldID) { }

		void Destroy()
		{
			for (auto& iterator : m_ComponentArrays)
				iterator.second.Destroy();
		}

		template<typename T>
		T* Add(EntityID id)
		{
			T* component = Get<T>(id);
			if(component)
				return component;
			
			type_index type = typeid(T);
			if (m_ComponentArrays.find(type) == m_ComponentArrays.end())
				m_ComponentArrays.emplace(type, ComponentData());
			component = (T*)malloc(sizeof(T)); // new T();
			m_ComponentArrays[type].Add(component, id);
			
			if (m_EntityComponents.find(id) == m_EntityComponents.end())
				m_EntityComponents.emplace(id, vector<type_index>());
			m_EntityComponents[id].push_back(type);

			string typeName = string(type.name());
			Utilities::MessageBus::eventBus()->Send("ComponentAdd" + string(type.name()), Utilities::DataStream().write(m_WorldID)->write(id));

			return component;
		}

		template<typename T1, typename T2>
		void Add(EntityID id)
		{
			Add<T1>(id);
			Add<T2>(id);
		}

		template<typename T1, typename T2, typename T3>
		void Add(EntityID id)
		{
			Add<T1>(id);
			Add<T2>(id);
			Add<T3>(id);
		}

		template<typename T1, typename T2, typename T3, typename T4>
		void Add(EntityID id)
		{
			Add<T1>(id);
			Add<T2>(id);
			Add<T3>(id);
			Add<T4>(id);
		}

		template<typename T>
		T* Get(EntityID id)
		{
			if (Empty(id))
				return nullptr;
			type_index type = typeid(T);
			return m_ComponentArrays[type].Get<T>(id);
		}

		template<typename T>
		unordered_map<EntityID, T*> Get()
		{
			type_index type = typeid(T);
			return m_ComponentArrays.find(type) == m_ComponentArrays.end() ?
				unordered_map<EntityID, T*>() : m_ComponentArrays[type].Get<T>();
		}

		template<typename T>
		unordered_map<EntityID, T*> Get(EntityID entities[], unsigned int entityCount)
		{
			type_index type = typeid(T);
			unordered_map<EntityID, T*> map;
			for (unsigned int i = 0; i < entityCount; i++)
			{
				T* component = m_ComponentArrays[type].Get<T>(entities[i]);
				if (component)
					map.emplace(entities[i], component);
			}
			return map;
		}

		template<typename T>
		unordered_map<EntityID, T*> Get(vector<EntityID> entities) { return Get<T>(entities.data(), (unsigned int)entities.size()); }

		template<typename T1, typename T2>
		unordered_map<EntityID, pair<T1*, T2*>> Get()
		{
			unordered_map<EntityID, T1*> type1 = Get<T1>();
			unordered_map<EntityID, T2*> type2 = Get<T2>();

			unordered_map<EntityID, pair<T1*, T2*>> finalMap;

			for (const auto& pair : type1)
			{
				if (type2.find(pair.first) != type2.end())
					finalMap.emplace(pair.first, make_pair(pair.second, type2[pair.first]));
			}

			return finalMap;
		}
		
		template<typename T>
		vector<EntityID> GetEntities()
		{
			PROFILE_FN();
			
			type_index type = typeid(T);
			return m_ComponentArrays.find(type) == m_ComponentArrays.end() ?
				vector<EntityID>() : m_ComponentArrays[type].entities();
		}

		bool Empty(EntityID id) { return m_EntityComponents.find(id) == m_EntityComponents.end() || m_EntityComponents[id].size() == 0; }

		template<typename T>
		bool Has(EntityID id)
		{
			type_index type = typeid(T);
			return m_ComponentArrays.find(type) != m_ComponentArrays.end() && m_ComponentArrays[type].Has(id);
		}

		template<typename T1, typename T2>
		bool Has(EntityID id) { return Has<T1>(id) && Has<T2>(id); }

		template<typename T1, typename T2, typename T3>
		bool Has(EntityID id) { return Has<T1>(id) && Has<T2>(id) && Has<T3>(id); }

		template<typename T1, typename T2, typename T3, typename T4>
		bool Has(EntityID id) { return Has<T1>(id) && Has<T2>(id) && Has<T3>(id) && Has<T4>(id); }

		template<typename T>
		void Remove(EntityID id)
		{
			PROFILE_FN()
			
			if (!Has<T>(id))
				return;
			type_index type = typeid(T);
			if (m_ComponentArrays.find(type) == m_ComponentArrays.end())
				return;

			vector<type_index>& components = m_EntityComponents[id];
			components.erase(remove(components.begin(), components.end(), type), components.end());
			m_ComponentArrays[type].Remove(id);

			Utilities::MessageBus::eventBus()->Send("ComponentRemove" + string(type.name()), Utilities::DataStream().write(m_WorldID)->write(id));
		}

		void Clear(EntityID id)
		{
			PROFILE_FN()
			if (Empty(id))
				return;

			for (const auto& type : m_EntityComponents[id])
			{
				ComponentData* componentArray = &m_ComponentArrays[type];
				m_ComponentArrays[type].Remove(id);
				Utilities::MessageBus::eventBus()->Send("ComponentRemove" + string(type.name()), Utilities::DataStream().write(m_WorldID)->write(id));
			}
			m_EntityComponents[id].clear();
		}
	};
}