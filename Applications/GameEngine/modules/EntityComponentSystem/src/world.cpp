#include <ECS/world.hpp>

using namespace ECS;

unordered_map<unsigned int, World*> World::s_Worlds;

World::World()
{
	m_ID = 0;
	while (s_Worlds.find(++m_ID) != s_Worlds.end());
	s_Worlds.emplace(m_ID, this);

	m_SystemManager = make_unique<SystemManager>();
	m_ComponentManager = make_unique<ComponentManager>(m_ID);
	m_EntityManager = make_unique<EntityManager>(m_ComponentManager.get(), m_ID);

	m_EntityManager->Prepare();
}

void World::Initialize()
{
	m_SystemManager->Initialize();
}

void World::Destroy()
{
	m_SystemManager->Destroy();
	m_ComponentManager->Destroy();

	s_Worlds.erase(m_ID);
}

void World::Update(float deltaTime) { m_SystemManager->Update(deltaTime); }

void World::Draw() { m_SystemManager->Draw(); }

void World::PrepareEntities(unsigned int count) { m_EntityManager->Prepare(count); }

Entity World::CreateEntity()
{
	Entity e = Entity(m_EntityManager->Create(), this);
	m_Entities.emplace(e.ID, e);
	return e;
}

Entity World::GetEntity(EntityID entity)
{
	if (m_Entities.find(entity) == m_Entities.end())
	{
		if (m_EntityManager->entities()[entity])
			m_Entities.emplace(entity, Entity(entity, this));
	}
	return m_Entities.at(entity);
}

void World::DestroyEntity(EntityID entity)
{
	m_EntityManager->Destroy(entity);
	if(m_Entities.find(entity) != m_Entities.end())
		m_Entities.erase(entity);
}

void World::ClearComponents(EntityID entity) { m_ComponentManager->Clear(entity); }

EntityID World::ID() { return m_ID; }
EntityManager* World::GetEntityManager() { return m_EntityManager.get(); }
SystemManager* World::GetSystemManager() { return m_SystemManager.get(); }
ComponentManager* World::GetComponentManager() { return m_ComponentManager.get(); }