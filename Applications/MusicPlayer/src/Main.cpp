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
#include <iostream>
#include <raylib.h>
#include <MusicPlayer.hpp>

#pragma warning(push, 0) // Disable warnings from raygui.h
#include <raygui.h>
#pragma warning(pop) // Restore warnings

using namespace std;
using namespace LCDS; // Lewis Comstive's Data Structures
using namespace MusicPlayer;

Player m_Player;

void DrawGUI(MusicPlayer::Player&, Font&);
void GetInput();

Font DefaultFont;
const Color ClearColor = { 33, 33, 38, 255 };

extern unsigned int FilepathHash(string&);

int main()
{
	vector<string> testFilepaths =
	{
		"test",
		"tist",
		"toast",
		"test.exe",
		"test.txt",
		"test.ext",
		"music/testing",
		"music/testingg",
		"music/testing.mp3",
		"music/testing.wav",
		"music/",
		"music/subdir/beep"
	};
	/*
	for (auto& filepath : testFilepaths)
		cout << filepath << " -> " << FilepathHash(filepath) << endl;
	*/
	/*
	HashTable<string, string> testTable(30, FilepathHash);
	cout << "Test Filepaths (" << testFilepaths.size() << ")" << endl;
	for(auto& filepath : testFilepaths)
		testTable.Add(filepath, filepath + "(" + to_string(FilepathHash(filepath)) + ")");
	testTable.Print();
	*/

	/*
	m_Player.GetPlaylist().Add(Song{ "Pass", "Ryzer", "./assets/music/NWordPass.mp3" });
	m_Player.GetPlaylist().Add(Song{ "Fireflies", "Ryzer", "./assets/music/Rickys_Fireflies.mp3" });
	m_Player.GetPlaylist().Add(Song{ "Mockingbird", "Ricky", "./assets/music/Mockingbird.mp3" });
	m_Player.GetPlaylist().Add(Song{ "Heman", "Ryzer", "./assets/music/Heman.mp3" });
	*/
	m_Player.GetPlaylist().Load();

	// Window
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	InitWindow(800, 600, "Music Player");

	// Audio
	InitAudioDevice();
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

void DrawCenteredText(string text, int x, int y, int fontSize = 16, Color color = RAYWHITE)
{
	int textWidth = MeasureText(text.c_str(), fontSize);
	DrawText(text.c_str(), x - textWidth / 2, y - fontSize / 2, fontSize, color);
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