#include <GameEngine/ResourceManager.hpp>
#include <GameUtilities/inline-profiler.hpp>

using namespace std;
using namespace LCDS;

unsigned int StringHash(string& input)
{
	unsigned int hash = 0;
	for (size_t i = 0; i < input.size(); i++)
		hash = (hash * 1313) + input[i];

	return hash;
}

// --- MAPS --- //
HashTable<ResourceID, Sound> ResourceManager::m_Sounds;
HashTable<ResourceID, Texture> ResourceManager::m_Textures;

HashTable<string, ResourceID> ResourceManager::m_ResourcePaths(25, StringHash);

// --- SOUND --- //
ResourceID ResourceManager::LoadSound(std::string filename)
{
	PROFILE_FN();
	if (m_ResourcePaths.Contains(filename))
		return m_ResourcePaths[filename];

	ResourceID id = GetNextResourceID();
	m_ResourcePaths.Add(filename, id);
	m_Sounds.Add(id, ::LoadSound(filename.c_str())); // Raylib
	GAME_LOG_DEBUG("Loaded sound [" + to_string(id) + "] " + filename);
	return id;
}

void ResourceManager::UnloadSound(ResourceID id)
{
	if (!m_Sounds.Contains(id))
		return; // Not loaded
	::UnloadSound(m_Sounds[id]); // Raylib
	m_Sounds.Remove(id);
}

Sound ResourceManager::GetSound(ResourceID id) { return (id == InvalidResourceID || !m_Sounds.Contains(id)) ? Sound{} : m_Sounds[id]; }

// --- TEXTURE --- //
ResourceID ResourceManager::LoadTexture(std::string filename)
{
	PROFILE_FN();
	if (m_ResourcePaths.Contains(filename))
		return m_ResourcePaths[filename];

	ResourceID id = GetNextResourceID();
	m_ResourcePaths.Add(filename, id);
	m_Textures.Add(id, ::LoadTexture(filename.c_str()));

	GAME_LOG_DEBUG("Loaded texture [" + to_string(id) + "] '" + filename + "'");

	return id;
}

void ResourceManager::UnloadTexture(ResourceID id)
{
	if (!m_Textures.Contains(id))
		return; // doesn't exist
	::UnloadTexture(m_Textures[id]); // Raylib
	m_Textures.Remove(id);
}

Texture ResourceManager::GetTexture(ResourceID id)
{
	return id == InvalidResourceID || !m_Textures.Contains(id) ? Texture{ 0 } : m_Textures[id];
}

// --- MISC. --- //
ResourceID ResourceManager::GetNextResourceID()
{
	PROFILE_FN()
	ResourceID ID = 0;
	while (m_Sounds.Contains(ID) || m_Textures.Contains(ID))
		ID++;
	return ID;
}

