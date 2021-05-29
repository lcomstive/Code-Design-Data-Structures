#pragma once

struct CameraComponent
{
	float Zoom;
	Rectangle Viewport;

	CameraComponent() : Zoom(1.0f), Viewport({ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }) { }
};