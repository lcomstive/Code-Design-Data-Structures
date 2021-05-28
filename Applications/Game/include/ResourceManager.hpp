#pragma once
#include <map>
#include <string>
#include <raylib.h>

typedef unsigned int ResourceID;
const ResourceID InvalidResourceID = (unsigned int)-1;

class ResourceManager
{
	static std::map<ResourceID, Sound> m_Sounds;
	static std::map<ResourceID, Texture> m_Textures;

	static std::map<std::string, ResourceID> m_ResourcePaths;

	static ResourceID GetNextResourceID();
public:
	const std::string AssetPath = "assets/";
	
	// --- SOUND --- //
	static Sound GetSound(ResourceID id);
	static void UnloadSound(ResourceID id);
	static ResourceID LoadSound(std::string filename);

	// --- TEXTURE --- //
	static Texture GetTexture(ResourceID id);
	static void UnloadTexture(ResourceID id);
	static ResourceID LoadTexture(std::string filename);
};