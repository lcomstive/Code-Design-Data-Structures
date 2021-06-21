#include <string> // to_string
#include <raylib.h>
#include <iostream>
#include <vector>
#include <cmath> // std::lerp
#include <DataStructures/BinaryTree.hpp>

#define RAYGUI_IMPLEMENTATION
#pragma warning(push, 0) // Disable warnings from raygui.h
#include <raygui.h>
#pragma warning(pop)

using namespace std;

bool DrawGUI(); // Returns true if mouse was captured by UI
void DrawTree();
void HandleMouse();
void HandleCameraMovement();

// Constants
const float CameraMoveSpeed = 300;
const float CameraZoomSpeed = 100;

// Globals
LCDS::BinaryTree<int> Tree;
Camera2D Cam;
int HighlightedValue = -1;
Vector2 LastMousePos = { 0, 0 };

int main()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 600, "Binary Tree Viewer");

	Cam = {};
	Cam.target = { 0, 0 };
	Cam.offset =
	{
		GetScreenWidth() / 2.0f,
		GetScreenHeight() / 2.0f,
	};
	Cam.zoom = 1.0f;

	LastMousePos = GetMousePosition();

	// Randomly generated values
	const vector<int> ExampleValues =
	{
		69, 40, 42, 21,
		99, 100, 80, 82,
		79, 60, 50, 55,
		44, 20, 105, 37,
		27, 85, 58, 97,
		4,	99,	50,	14,	60,
		24,	13,	53,	78,	98,
		48,	100,25,	82,	1,
		88,	2,	84,	8,	51,
	};
	for (int value : ExampleValues)
		Tree.Insert(value);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground({ 37, 37, 43, 255 });
		DrawFPS(10, GetScreenHeight() - 30);

		HandleCameraMovement();

		BeginMode2D(Cam);
		DrawTree();
		EndMode2D();

		if (!DrawGUI()) // returns true if mouse clicked on UI
			HandleMouse();

		EndDrawing();
	}

	return 0;
}

int InsertValue = 0;
bool InsertValueSpinnerEdit = false;

bool DrawGUI()
{
	// Insert node value
	if (GuiValueBox({ 10, 10, 50, 30 }, nullptr, &InsertValue, 0, 9999, InsertValueSpinnerEdit))
		InsertValueSpinnerEdit = !InsertValueSpinnerEdit;

	if (GuiButton({ 70, 10, 30, 30 }, "+"))
	{
		Tree.Insert(InsertValue);
		cout << "Inserted " << InsertValue << endl;
		InsertValue = 0;
	}
	if (GuiButton({ 110, 10, 50, 30 }, "Find"))
	{
		auto foundNode = Tree.Search(InsertValue);
		HighlightedValue = foundNode ? foundNode->Value : -1;
		cout << "Searched for " << InsertValue << (foundNode ? "" : " (not found)") << endl;
	}

	return false;
}

void HandleMouse()
{
	Vector2 currentMousePos = GetMousePosition();
	Vector2 mouseDelta =
	{
		LastMousePos.x - currentMousePos.x,
		LastMousePos.y - currentMousePos.y
	};
	LastMousePos = currentMousePos;

	// Move camera with mouse left
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Cam.target.x += mouseDelta.x;
		Cam.target.y += mouseDelta.y;
	}

	Cam.zoom += GetMouseWheelMove() * CameraZoomSpeed * GetFrameTime();
	if (Cam.zoom < 0.1f) Cam.zoom = 0.1f;
	if (Cam.zoom > 10) Cam.zoom = 10;
}

void HandleCameraMovement()
{
	auto deltaTime = GetFrameTime();
	Vector2 movement = { 0, 0 };
	if (IsKeyDown(KEY_UP))	  movement.y -= CameraMoveSpeed * deltaTime;
	if (IsKeyDown(KEY_DOWN))  movement.y += CameraMoveSpeed * deltaTime;
	if (IsKeyDown(KEY_LEFT))  movement.x -= CameraMoveSpeed * deltaTime;
	if (IsKeyDown(KEY_RIGHT)) movement.x += CameraMoveSpeed * deltaTime;

	Cam.target =
	{
		Cam.target.x + movement.x,
		Cam.target.y + movement.y
	};
}

const Color NormalNodeColourLeft  = { 127, 93, 244, 255 };
const Color NormalNodeColourRight = { 93, 227, 104, 255 };
const Color HighlightNodeColour = { 255, 200, 200, 255 };

const int WidthBetweenNodes = 50;
const int InitialVerticalSpacing = 200;

unsigned char ByteLerp(unsigned char a, unsigned char b, float value) { return (unsigned char)(a + value * (b - a)); }

Color ColorLerp(Color a, Color b, float value)
{
	return
	{
		ByteLerp(a.r, b.r, value),
		ByteLerp(a.g, b.g, value),
		ByteLerp(a.b, b.b, value),
		ByteLerp(a.a, b.a, value)
	};
}

void DrawTreeNode(LCDS::BinaryTreeNode<int>* currentNode, int x, int y, float verticalSpacing)
{
	if (!currentNode)
		return;

	if (currentNode->Left)
	{
		DrawLine(x, y, x + WidthBetweenNodes, (int)(y + verticalSpacing), GRAY);
		DrawTreeNode(currentNode->Left, x + WidthBetweenNodes, y + verticalSpacing, verticalSpacing * 0.6f);
	}

	if (currentNode->Right)
	{
		DrawLine(x, y, x + WidthBetweenNodes, (int)(y - verticalSpacing), GRAY);
		DrawTreeNode(currentNode->Right, x + WidthBetweenNodes, y - verticalSpacing, verticalSpacing * 0.6f);
	}

	float lerpModifier = GetTime() + Tree.GetDepth(currentNode) / (float)Tree.GetDepth();
	lerpModifier = (sin(lerpModifier) + 1) * 0.5f;  // Remap sin from -1 <-> -1 to 0 <-> 1

	Color nodeColor = ColorLerp(
		NormalNodeColourLeft,
		NormalNodeColourRight,
		lerpModifier
	);
	DrawCircle(x, y, 10, HighlightedValue == currentNode->Value ? HighlightNodeColour : nodeColor);
	DrawText(to_string(currentNode->Value).c_str(), x, y, 10, RAYWHITE);
}

void DrawTree()
{
	DrawTreeNode(Tree.GetRoot(), 0, 0, InitialVerticalSpacing);
}
