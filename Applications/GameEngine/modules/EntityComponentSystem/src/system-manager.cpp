#include <iostream>
#include <ECS/system-manager.hpp>
#include <GameUtilities/inline-profiler.hpp>

using namespace std;
using namespace ECS;

SystemManager::SystemManager() : m_Systems(map<type_index, System*>()), m_Initialized(false) { }

void SystemManager::Initialize()
{
	if(m_Initialized)
		return;
	m_Initialized = true;
	for (auto& iterator : m_Systems)
		iterator.second->Init();
}

void SystemManager::Destroy()
{
	for (auto& iterator : m_Systems)
	{
		iterator.second->Destroy();
		delete iterator.second;
	}
	m_Systems.clear();
}

void SystemManager::Update(float deltaTime)
{
	PROFILE_FN();
	for (auto& pair : m_Systems)
		pair.second->Update(deltaTime);
}

void SystemManager::Draw()
{
	PROFILE_FN();
	for (auto& pair : m_Systems)
		pair.second->Draw();
}

vector<System*> SystemManager::all()
{
	vector<System*> systems;
	for (auto& pair : m_Systems)
		systems.push_back(pair.second);
	return systems;
}