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
#include <vector>
#include <raylib.h>
#include "WinInc.h" // Windows-specific includes and defines

struct Entity
{
	float x = 0, y = 0;
	float rotation = 0;
	float speed = 0;
	unsigned char r = 0, g = 0, b = 0;
	float size = 1;
};

class EntityDisplayApp
{
public:
	EntityDisplayApp(int screenWidth = 800, int screenHeight = 450);
	~EntityDisplayApp();

	bool Startup();
	void Shutdown();

	void Update(float deltaTime); // App logic
	void Draw();				  // Render to screen

protected:
	int m_screenWidth;
	int m_screenHeight;

	// NOTE: m_Entities and m_EntityCount are shared memory

	// Array of entities
	Entity* m_Entities;

	// Amount of entities in array
	unsigned int* m_EntityCount;

#if _WIN32
	// Windows-specific handle to shared memory
	HANDLE m_SharedMemHandle;
#else
	// TODO: Cross-platform implementation
#endif
};