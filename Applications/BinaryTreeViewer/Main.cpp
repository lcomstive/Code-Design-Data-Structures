#include <string> // to_string
#include <raylib.h>
#include <iostream>
#include <DataStructures/BinaryTree.hpp>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

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
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
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

	Tree.Insert(69);
	Tree.Insert(40);
	Tree.Insert(42);
	Tree.Insert(21);
	Tree.Insert(99);
	Tree.Insert(100);
	Tree.Insert(80);
	Tree.Insert(82);

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

const Color NormalNodeColour = GRAY;
const Color HighlightNodeColour = { 255, 200, 200, 255 };

const int WidthBetweenNodes = 30;
const int NodeHeightScaling = 10;

void DrawTreeNode(LCDS::BinaryTreeNode<int>* currentNode, int x, int y, int depth)
{
	if (!currentNode)
		return;

	if (currentNode->Left)
	{
		DrawLine(x, y, x + WidthBetweenNodes, y + NodeHeightScaling * depth, GRAY);
		DrawTreeNode(currentNode->Left, x + WidthBetweenNodes, y + NodeHeightScaling * depth, depth - 1);
	}

	if (currentNode->Right)
	{
		DrawLine(x, y, x + WidthBetweenNodes, y - NodeHeightScaling * depth, GRAY);
		DrawTreeNode(currentNode->Right, x + WidthBetweenNodes, y - NodeHeightScaling * depth, depth - 1);
	}

	DrawCircle(x, y, 10, HighlightedValue == currentNode->Value ? HighlightNodeColour : NormalNodeColour);
	DrawText(to_string(currentNode->Value).c_str(), x, y, 10, RAYWHITE);
}

void DrawTree()
{
	DrawTreeNode(Tree.GetRoot(), 0, 0, Tree.GetDepth());
}
