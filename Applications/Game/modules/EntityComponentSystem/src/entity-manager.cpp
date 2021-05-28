#include <iostream>
#include <ECS/world.hpp>
#include <ECS/entity-manager.hpp>

using namespace std;
using namespace ECS;
using namespace Utilities;

EntityManager::EntityManager(ComponentManager* componentManager, unsigned int worldID) : m_ComponentManager(componentManager), m_WorldID(worldID), m_Entities(nullptr) { }

void EntityManager::Destroy() { delete[] m_Entities; }

EntityID EntityManager::Create()
{
	PROFILE_FN();

	EntityID id = NextAvailableEntityID();
	m_Entities[id] = false;
	m_AvailableEntityCount--;
	
	MessageBus::eventBus()->Send("EntityCreated", DataStream().write(id)->write(m_WorldID));
	return id;
}

void EntityManager::Destroy(EntityID id)
{
	PROFILE_FN();

	m_ComponentManager->Clear(id);
	m_Entities[id] = true;
	m_AvailableEntityCount++;

	MessageBus::eventBus()->Send("EntityDestroyed", DataStream().write(id)->write(m_WorldID));
}

vector<EntityID> EntityManager::entities() const
{
	PROFILE_FN()

	vector<EntityID> entities;
	for (EntityID i = 0; i < m_EntityCount; i++)
		if (!m_Entities[i])
			entities.emplace_back(i);
	return entities;
}

void EntityManager::Prepare(unsigned int count)
{
	PROFILE_FN();

	EntityID id = 0;
	unsigned int spareIDs = 0;
	for (id = 0; id < m_EntityCount; id++)
	{
		if (!m_Entities[id])
		{
			spareIDs++;
			if (spareIDs >= count)
				return;
		}
	}

	AddChunk(count - spareIDs);
}

EntityID EntityManager::NextAvailableEntityID()
{
	PROFILE_FN();

	EntityID id = 0;
	for (id = 0; id < m_EntityCount; id++)
		if (m_Entities[id])
			return id;
	AddChunk();
	return id;
}

void EntityManager::AddChunk(unsigned int chunkSize)
{
	PROFILE_FN();
	GAME_LOG_DEBUG("Adding chunk (currently " + to_string(m_EntityCount) + " GetEntities)");

	bool* oldEntities = m_Entities;
	m_Entities = new bool[m_EntityCount + chunkSize];
	memcpy(m_Entities, oldEntities, m_EntityCount);
	delete[] oldEntities;

	for (unsigned int i = m_EntityCount; i < m_EntityCount + chunkSize; i++)
		m_Entities[i] = true;

	m_EntityCount += chunkSize;
	m_AvailableEntityCount += chunkSize;

	GAME_LOG_DEBUG("Added chunk of size " + to_string(chunkSize) + " GetEntities (" + to_string(m_EntityCount) + " total, " + to_string(m_AvailableEntityCount) + " available)");
}

void EntityManager::RemoveChunk()
{
	/// TODO: Remove trailing available GetEntities
}