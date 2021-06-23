/*
 *
 * AIE Introduction to C++
 * Playlist Viewer - Demonstration of double linked lists
 * Lewis Comstive (s210314)
 *
 * See the LICENSE file in the root directory of project for copyright.
 *
 */
#include <string>
#include <raylib.h>
#include <MusicPlayer.hpp>

#pragma warning(push, 0) // Disable warnings from raygui.h
#include <raygui.h>
#pragma warning(pop) // Restore warnings

using namespace std;
using namespace LCDS; // Lewis Comstive's Data Structures
using namespace MusicPlayer;

// Globals
Player m_Player;
Font DefaultFont;
const Color ClearColor = { 33, 33, 38, 255 };

extern unsigned int FilepathHash(string&);

void DrawGUI(MusicPlayer::Player&, Font&);
void GetInput();

int main()
{
	// Window
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	InitWindow(800, 600, "Music Player");

	// Audio
	InitAudioDevice();
	if (!m_Player.GetPlaylist().Load()) // Load playlist from file
	{
		// File doesn't exist, or error ocurred
		// Add some default songs
		m_Player.GetPlaylist().Add({ "He-Man", "RiZer", "./assets/music/Heman.mp3" });
		m_Player.GetPlaylist().Add({ "Mockingbird", "RiZer", "./assets/music/Mockingbird.mp3" });
		m_Player.GetPlaylist().Add({ "Fireflies", "Ricky", "./assets/music/Rickys_Fireflies.mp3" });
	}

	// Load first playlist song into memory
	m_Player.Load();

	// GUI
	DefaultFont = LoadFont("./assets/OpenSans-Regular.ttf");
	GuiLoadStyle("assets/theme.rgs");
	GuiSetFont(DefaultFont);

	while (!WindowShouldClose())
	{
		GetInput();

		BeginDrawing();
		ClearBackground(ClearColor);

		DrawGUI(m_Player, DefaultFont);

		EndDrawing();

		m_Player.Update();
	}

	m_Player.GetPlaylist().Save();
	return 0;
}

void GetInput()
{
	auto currentSong = m_Player.GetPlayingSong();

	/*

	MUSIC PLAYER CONTROLS

	Right Arrow  -  Next Song
	Left  Arrow  -  Previous Song
	Space		 -  Pause/Play
	M			 -  Mute/Unmute

	*/

	if (IsKeyPressed(KEY_RIGHT)) m_Player.Next();
	if (IsKeyPressed(KEY_LEFT)) m_Player.Previous();
	if (IsKeyPressed(KEY_SPACE)) m_Player.TogglePause();
	if (IsKeyPressed(KEY_M)) m_Player.ToggleMute();
}