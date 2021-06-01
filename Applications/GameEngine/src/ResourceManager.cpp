#include <GameEngine/ResourceManager.hpp>
#include <GameUtilities/inline-profiler.hpp>

using namespace std;

// --- MAPS --- //
map<ResourceID, Sound> ResourceManager::m_Sounds;
map<ResourceID, Texture> ResourceManager::m_Textures;

map<string, ResourceID> ResourceManager::m_ResourcePaths;

// --- SOUND --- //
ResourceID ResourceManager::LoadSound(std::string filename)
{
	PROFILE_FN();
	if(m_ResourcePaths.find(filename) != m_ResourcePaths.end())
		return m_ResourcePaths[filename];

	ResourceID id = GetNextResourceID();
	m_ResourcePaths.emplace(filename, id);
	m_Sounds.emplace(id, ::LoadSound(filename.c_str())); // Raylib
	return id;
}

void ResourceManager::UnloadSound(ResourceID id)
{
	if(m_Sounds.find(id) == m_Sounds.end())
		return; // Not loaded
	::UnloadSound(m_Sounds[id]); // Raylib
	m_Sounds.erase(id);
}

Sound ResourceManager::GetSound(ResourceID id)
{
	return m_Sounds.find(id) == m_Sounds.end() ? Sound {} : m_Sounds[id];
}

// --- TEXTURE --- //
ResourceID ResourceManager::LoadTexture(std::string filename)
{
	PROFILE_FN();
	if(m_ResourcePaths.find(filename) != m_ResourcePaths.end())
		return m_ResourcePaths[filename];

	ResourceID id = GetNextResourceID();
	m_ResourcePaths.emplace(filename, id);
	m_Textures.emplace(id, ::LoadTexture(filename.c_str()));

	GAME_LOG_DEBUG("Loaded texture [" + to_string(id) + "] '" + filename + "'");
	
	return id;
}

void ResourceManager::UnloadTexture(ResourceID id)
{
	if(m_Textures.find(id) == m_Textures.end())
		return; // doesn't exist
	::UnloadTexture(m_Textures[id]); // Raylib
	m_Textures.erase(id);
}

Texture ResourceManager::GetTexture(ResourceID id)
{
	return m_Textures.find(id) == m_Textures.end() ? Texture { 0 } : m_Textures[id];
}

// --- MISC. --- //
ResourceID ResourceManager::GetNextResourceID()
{
	PROFILE_FN()
	ResourceID ID = 0;
	while(m_Sounds.find(ID) != m_Sounds.end() ||
		  m_Textures.find(ID) != m_Textures.end())
		ID++;
	return ID;
}

