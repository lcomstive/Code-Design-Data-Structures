#pragma once
#include <raylib.h>

struct CameraComponent
{
	float Zoom;
	Rectangle Viewport;

	CameraComponent() : Zoom(1.0f)
	{
		Vector2 dpi = GetWindowScaleDPI();
		Viewport =
			{
			0, 0, // (x, y)
			GetScreenWidth() * dpi.x,
			GetScreenHeight() * dpi.y
		};
	}
};