/*
 *
 * AIE Code Design & Data Structures
 * Data Structures
 * Lewis Comstive (s210314)
 *
 * See the LICENSE file in the root directory of project for copyright information.
 *
 */
#pragma once

#pragma warning(push, 0) // Disable warnings from Raylib
#include <raylib.h>
#pragma warning(pop) // Restore warnings

#include "WinInc.h" // Windows-specific includes and defines

struct Entity
{
	float x = 0, y = 0;
	float rotation = 0;
	float speed = 0;
	unsigned char r = 0, g = 0, b = 0;
	float size = 1;
};

class EntityEditorApp
{
public:
	EntityEditorApp(int screenWidth = 800, int screenHeight = 450);
	~EntityEditorApp();

	bool Startup();
	void Shutdown();

	void Update(float deltaTime);
	void Draw();

protected:
	// Initial count of entities
	static const unsigned int ENTITY_COUNT = 10;

	int m_ScreenWidth;
	int m_ScreenHeight;

	// NOTE: m_Entities and m_SharedEntityCount are shared memory
	Entity* m_Entities;					// Array of entities
	unsigned int* m_SharedEntityCount;  // Count of entities in array

#if _WIN32
	// Windows-specific handle to shared memory
	HANDLE m_SharedMemHandle;
#else
	// TODO: Cross-platform implementation
#endif
};