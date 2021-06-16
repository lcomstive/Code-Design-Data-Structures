#include <iostream>
#include "EntityDisplayApp.h"

using namespace std;

EntityDisplayApp::EntityDisplayApp(int screenWidth, int screenHeight) :
	m_Entities(nullptr),
	m_EntityCount(nullptr),
	m_screenWidth(screenWidth),
	m_screenHeight(screenHeight) { }

EntityDisplayApp::~EntityDisplayApp() {

}

bool EntityDisplayApp::Startup()
{
#if _WIN32
	// Get shared memory
	m_SharedMemHandle = OpenFileMapping(
		FILE_MAP_READ,	// Read/Write access
		FALSE,			// Can children processess inherit this handle?
		"EntityEditorApp_SharedEntity"); // Handle name

	// Check that memory was accessed
	if (!m_SharedMemHandle)
	{
		cerr << "Failed to open shared memory" << endl;
		return false;
	}

	// Map the memory, which starts with an unsigned int
	m_EntityCount = (unsigned int*)MapViewOfFile(m_SharedMemHandle, FILE_MAP_READ, 0, 0, sizeof(unsigned int));

	// Check the count was mapped
	if (!m_EntityCount)
	{
		cerr << "Failed to map entity count" << endl;
		return false;
	}

	// Map the entities from the shared memory
	// Note that no offset is given, this is because Windows requires the offset to be
	//	a multiple of the allocation granularity (e.g. 64KB).
	//  To counter this, the memory is mapped at beginning then the pointer is moved the appropriate offset
	m_Entities = (Entity*)MapViewOfFile(m_SharedMemHandle, FILE_MAP_READ, 0, 0, sizeof(Entity) * *m_EntityCount);
	m_Entities += sizeof(unsigned int); // Move pointer offset by size of unsigned int (the entity count)
#else

#endif

	InitWindow(m_screenWidth, m_screenHeight, "EntityDisplayApp");
	SetTargetFPS(60);

	return true;
}

void EntityDisplayApp::Shutdown()
{
#if _WIN32
	CloseHandle(m_SharedMemHandle);
#else

#endif

	CloseWindow();        // Close window and OpenGL context
}

void EntityDisplayApp::Update(float deltaTime)
{

}

void EntityDisplayApp::Draw() {
	BeginDrawing();

	ClearBackground(RAYWHITE);

	// draw entities
	for (unsigned int i = 0; i < *m_EntityCount; i++)
	{
		Entity& entity = m_Entities[i];
		DrawRectanglePro(
			Rectangle{ entity.x, entity.y, entity.size, entity.size }, // rectangle
			Vector2{ entity.size / 2, entity.size / 2 }, // origin
			entity.rotation,
			Color{ entity.r, entity.g, entity.b, 255 });
	}

	// output some text, uses the last used colour
	DrawText("Press ESC to quit", 630, 15, 12, LIGHTGRAY);

	EndDrawing();
}