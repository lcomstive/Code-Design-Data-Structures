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
#define RAYGUI_SUPPORT_ICONS
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#pragma warning(pop) // Restore warnings

using namespace std;
using namespace LCDS; // Lewis Comstive's Data Structures
using namespace MusicPlayer;

Player m_Player;

void DrawGUI();
void GetInput();

const Color ClearColor = { 33, 33, 38, 255 };

int main()
{
	// ReadFile();

	m_Player.GetPlaylist().Add(Song { "NWord Pass", "Ryzer", "music/NWordPass.mp3" });
	m_Player.GetPlaylist().Add(Song { "Fireflies", "Ryzer", "music/Rickys_Fireflies.mp3" });

	// Window
	InitWindow(800, 600, "Music Player");
	SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

	// Audio
	InitAudioDevice();
	m_Player.Play();

	// GUI
	GuiLoadStyle("assets/theme.rgs");

	while (!WindowShouldClose())
	{
		GetInput();

		BeginDrawing();
		ClearBackground(ClearColor);

		DrawGUI();

		EndDrawing();

		m_Player.Update();
	}

	return 0;
}

void DrawCenteredText(string text, int x, int y, int fontSize = 16, Color color = RAYWHITE)
{
	int textWidth = MeasureText(text.c_str(), fontSize);
	DrawText(text.c_str(), x - textWidth / 2, y - fontSize / 2, fontSize, color);
}

string GetTimecode(float time)
{
	int minutes = (int)floor(time / 60.0f);
	int seconds = (int)time % 60;
	return
		(minutes > 0 ? to_string(minutes) : "0") + ":" +
		(seconds >= 10 ? to_string(seconds) : "0" + (seconds > 0 ? to_string(seconds) : "0"));
}

void DrawGUI()
{
	Vector2 center = { GetScreenWidth() / 2, GetScreenHeight() / 2 };

	auto currentSong = m_Player.GetPlayingSong();

	DrawCenteredText((currentSong ? currentSong->Value.Name   : "-").c_str(), center.x, center.y - 60, 36, RAYWHITE);
	DrawCenteredText((currentSong ? currentSong->Value.Artist : "-").c_str(), center.x, center.y - 24, 24, GRAY);

	const float ProgressSliderWidthMax = 600;
	float progressSliderWidth = min(GetScreenWidth() * 0.6f, ProgressSliderWidthMax);

	float currentTime = m_Player.GetTimePlayed();
	float maxSongTime = m_Player.GetTotalSongTime();
	float songSeek = GuiSliderBar(
		{
			center.x - progressSliderWidth / 2.0f,
			center.y + 15,
			progressSliderWidth,
			7.5f
		},
		GetTimecode(currentTime).c_str(),
		GetTimecode(maxSongTime).c_str(),
		currentTime, 0, maxSongTime);

	if (songSeek != currentTime)
	{
		// SEEK
	}
}

void GetInput()
{
	auto currentSong = m_Player.GetPlayingSong();
	
	if (IsKeyPressed(KEY_RIGHT)) m_Player.Next();
	if (IsKeyPressed(KEY_LEFT)) m_Player.Previous();
	if (IsKeyPressed(KEY_SPACE)) m_Player.TogglePause();
}