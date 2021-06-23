/*
 *
 * AIE Code Design & Data Structures
 * Data Structures
 * Lewis Comstive (s210314)
 *
 * See the LICENSE file in the root directory of project for copyright information.
 *
 */

#include <raylib.h>
#include "EntityDisplayApp.h"

int main(int argc, char* argv[])
{
	float deltaTime = 0;
	EntityDisplayApp app(800, 450);

	// Initialisation
	if (!app.Startup())
		return -1;

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		// Update
		app.Update(GetFrameTime());

		// Draw
		app.Draw();
	}

	app.Shutdown();

	return 0;
}