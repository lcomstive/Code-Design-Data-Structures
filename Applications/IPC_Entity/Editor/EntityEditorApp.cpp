#include "EntityEditorApp.h"
#include <random>
#include <time.h>
#include <string>
#include <iostream>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#pragma warning(push, 0) // Disable warnings in Visual Studio until pop
#include <raygui.h>
#pragma warning(pop)

using namespace std;

EntityEditorApp::EntityEditorApp(int screenWidth, int screenHeight) : m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight) {

}

EntityEditorApp::~EntityEditorApp() {

}

#if _WIN32
// Reference to https://stackoverflow.com/a/17387176
string GetWindowsError()
{
	auto errorID = GetLastError();
	if (errorID == NO_ERROR)
		return string();

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, errorID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&messageBuffer, 0, nullptr
	);

	string finalMessage(messageBuffer, size);
	LocalFree(messageBuffer);
	return finalMessage;
}
#endif

bool EntityEditorApp::Startup() {

	InitWindow(m_ScreenWidth, m_ScreenHeight, "EntityDisplayApp");
	SetTargetFPS(60);

	unsigned int sharedMemorySize = sizeof(unsigned int) + sizeof(Entity) * ENTITY_COUNT;
#if _WIN32
	// Create shared memory
	m_SharedMemHandle = CreateFileMapping(
		INVALID_HANDLE_VALUE,			  // Existing virtual file
		nullptr,						  // Security attributes
		PAGE_READWRITE,					  // Access control
		0, sharedMemorySize, // Offset and size of memory block
		"EntityEditorApp_SharedEntity"    // Name of shared memory
	);

	// Check for errors
	auto error = GetLastError();
	if (error != NO_ERROR)
	{
		cerr << "Error creating shared memory: " << GetWindowsError() << endl;
		return false;
	}

	// Map the memory, which starts with an unsigned int
	m_SharedEntityCount = (unsigned int*)MapViewOfFile(m_SharedMemHandle, FILE_MAP_WRITE, 0, 0, sizeof(unsigned int));
	*m_SharedEntityCount = ENTITY_COUNT; // Setting shared memory to initial value

	// Map the entities to the shared memory
	// Note that no offset is given, this is because Windows requires the offset to be
	//	a multiple of the allocation granularity (e.g. 64KB).
	//  To counter this, the memory is mapped at beginning then the pointer is moved the appropriate offset
	m_Entities = (Entity*)MapViewOfFile(m_SharedMemHandle, FILE_MAP_WRITE, 0, 0, sizeof(Entity) * ENTITY_COUNT);
	m_Entities += sizeof(unsigned int); // Move pointer offset by size of unsigned int (the entity count)

	// Check for errors
	error = GetLastError();
	if (error != NO_ERROR)
	{
		cerr << "Error mapping shared memory: " << GetWindowsError() << endl;
		return false;
	}
#else

#endif

	// Generate random entities
	srand((unsigned int)time(nullptr));
	for(int i = 0; i < ENTITY_COUNT; i++)
	{
		m_Entities[i].x = (float)(rand() % m_ScreenWidth);
		m_Entities[i].y = (float)(rand() % m_ScreenHeight);
		m_Entities[i].size = 10.0f;
		m_Entities[i].speed = (float)(rand() % 100);
		m_Entities[i].rotation = (float)(rand() % 360);
		m_Entities[i].r = rand() % 255;
		m_Entities[i].g = rand() % 255;
		m_Entities[i].b = rand() % 255;
	}

	return true;
}

void EntityEditorApp::Shutdown() {

	CloseWindow();        // Close window and OpenGL context
}

void EntityEditorApp::Update(float deltaTime) {


	// select an entity to edit
	static int selection = 0;
	static bool selectionEditMode = false;
	static bool xEditMode = false;
	static bool yEditMode = false;
	static bool rotationEditMode = false;
	static bool sizeEditMode = false;
	static bool speedEditMode = false;
	static Color colorPickerValue = WHITE;


	if (GuiSpinner(Rectangle{ 90, 25, 125, 25 }, "Entity", &selection, 0, ENTITY_COUNT - 1, selectionEditMode)) selectionEditMode = !selectionEditMode;

	int intX = (int)m_Entities[selection].x;
	int intY = (int)m_Entities[selection].y;
	int intRotation = (int)m_Entities[selection].rotation;
	int intSize = (int)m_Entities[selection].size;
	int intSpeed = (int)m_Entities[selection].speed;


	// display editable stats within a GUI	
	GuiGroupBox(Rectangle{ 25, 70, 480, 220 }, "Entity Properties");

	if (GuiValueBox(Rectangle{ 90, 90, 125, 25 }, "x", &intX, 0, m_ScreenWidth, xEditMode)) xEditMode = !xEditMode;
	m_Entities[selection].x = (float)intX;

	if (GuiValueBox(Rectangle{ 90, 120, 125, 25 }, "y", &intY, 0, m_ScreenHeight, yEditMode)) yEditMode = !yEditMode;
	m_Entities[selection].y = (float)intY;

	m_Entities[selection].rotation = GuiSlider(Rectangle{ 90, 150, 125, 25 }, "rotation", TextFormat("%2.2f", m_Entities[selection].rotation), m_Entities[selection].rotation, 0, 360);
	m_Entities[selection].size = GuiSlider(Rectangle{ 90, 180, 125, 25 }, "size", TextFormat("%2.2f", m_Entities[selection].size), m_Entities[selection].size, 0, 100);
	m_Entities[selection].speed = GuiSlider(Rectangle{ 90, 210, 125, 25 }, "speed", TextFormat("%2.2f", m_Entities[selection].speed), m_Entities[selection].speed, 0, 100);

	colorPickerValue = GuiColorPicker(Rectangle{ 260, 90, 156, 162 }, Color{ m_Entities[selection].r, m_Entities[selection].g, m_Entities[selection].b });
	m_Entities[selection].r = colorPickerValue.r;
	m_Entities[selection].g = colorPickerValue.g;
	m_Entities[selection].b = colorPickerValue.b;


	// move entities

	for (int i = 0; i < ENTITY_COUNT; i++) {
		if (selection == i)
			continue;

		float s = sinf(m_Entities[i].rotation) * m_Entities[i].speed;
		float c = cosf(m_Entities[i].rotation) * m_Entities[i].speed;
		m_Entities[i].x -= s * deltaTime;
		m_Entities[i].y += c * deltaTime;

		// wrap position around the screen
		m_Entities[i].x = (float)fmod(m_Entities[i].x, m_ScreenWidth);
		if (m_Entities[i].x < 0)
			m_Entities[i].x += m_ScreenWidth;
		m_Entities[i].y = (float)fmod(m_Entities[i].y, m_ScreenHeight);
		if (m_Entities[i].y < 0)
			m_Entities[i].y += m_ScreenHeight;
	}
}

void EntityEditorApp::Draw() {
	BeginDrawing();

	ClearBackground(RAYWHITE);

	// draw entities
	for(int i = 0; i < ENTITY_COUNT; i++)
	{
		Entity& entity = m_Entities[i];
		DrawRectanglePro(
			Rectangle{ entity.x, entity.y, entity.size, entity.size }, // rectangle
			Vector2  { entity.size / 2, entity.size / 2 }, // origin
			entity.rotation,
			Color { entity.r, entity.g, entity.b, 255 });
	}

	// output some text, uses the last used colour
	DrawText("Press ESC to quit", 630, 15, 12, LIGHTGRAY);

	EndDrawing();
}