#pragma once

#pragma warning(push, 0) // Disable warnings in Visual Studio until pop
#include <raylib.h>
#pragma warning(pop)

#include "WinInc.h"

struct Entity {
	float x = 0, y = 0;
	float rotation = 0;
	float speed = 0;
	unsigned char r = 0, g = 0, b = 0;
	float size = 1;
};

class EntityEditorApp {
public:
	EntityEditorApp(int screenWidth = 800, int screenHeight = 450);
	~EntityEditorApp();

	bool Startup();
	void Shutdown();

	void Update(float deltaTime);
	void Draw();

protected:
	static const unsigned int ENTITY_COUNT = 10;

	int m_ScreenWidth;
	int m_ScreenHeight;

	// NOTE: m_Entities and m_SharedEntityCount are shared memory
	Entity* m_Entities;					// Array of entities
	unsigned int* m_SharedEntityCount;  // Count of entities in array

#if _WIN32
	HANDLE m_SharedMemHandle;
#else

#endif
};