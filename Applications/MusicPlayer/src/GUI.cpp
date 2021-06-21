#include <string>
#include <MusicPlayer.hpp>

#pragma warning(push, 0) // Disable warnings from raygui.h
#define RAYGUI_SUPPORT_ICONS
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#pragma warning(pop) // Restore warnings

using namespace std;

string GetTimecode(float time)
{
	int minutes = (int)floor(time / 60.0f);
	int seconds = (int)time % 60;
	return
		(minutes > 0 ? to_string(minutes) : "0") + ":" +
		(seconds >= 10 ? to_string(seconds) : "0" + (seconds > 0 ? to_string(seconds) : "0"));
}

void DrawPlaylistUI(MusicPlayer::Player& player, Font& font)
{
	auto& playlist = player.GetPlaylist();
	auto song = playlist.GetFirst();
	auto mousePos = GetMousePosition();

	Vector2 resolution = { (float)GetScreenWidth(), (float)GetScreenHeight() };

	static int playlistScrollValue = 0;
	playlistScrollValue = GuiScrollBar({ resolution.x - 30, 20, 10, resolution.y - 130 }, playlistScrollValue, 0, 100);

	auto dpi = GetWindowScaleDPI();
	if(dpi.x == 1 && dpi.y == 1)
		dpi = { 0, 0 }; // No need for shifting by resolution if dpi is { 1, 1 }
	BeginScissorMode(20 * dpi.x, -(resolution.y / 2) * dpi.y + 20, (resolution.x - 60) * dpi.x, (resolution.y - 150) * dpi.y);

	int songIndex = 0;
	while (song)
	{
		bool currentSong = song == player.GetPlayingSong();
		float yValue = 20 - 20.0f * playlistScrollValue + 65 * songIndex;

		// Dummy button for highlight when hovering
		Rectangle backgroundSize = { 20, yValue, resolution.x - 50, 65 };
		bool mouseHover = mousePos.x > backgroundSize.x && mousePos.x < backgroundSize.x + backgroundSize.width &&
						  mousePos.y > backgroundSize.y && mousePos.y < backgroundSize.y + backgroundSize.height;
		GuiDrawRectangle(backgroundSize, 0, RAYWHITE, Color{ 255, 255, 255, (unsigned char)(mouseHover ? 10 : 0) });

		GuiDrawText(song->Value.Name.c_str(), { 20, yValue }, 26, currentSong ? PURPLE : RAYWHITE);
		GuiDrawText(song->Value.Artist.c_str(), { 20, yValue + 20 }, 26, GRAY);

		DrawLine(backgroundSize.x, yValue + 60, backgroundSize.width, yValue + 60, Color{ 255, 255, 255, 20 });

		if (GuiButton({ backgroundSize.x + backgroundSize.width - 50, backgroundSize.y + 15, 30, 30 }, GuiIconText(RICON_PLAYER_PLAY, "")))
			player.Play(songIndex);
		if (GuiButton({ backgroundSize.x + backgroundSize.width - 90, backgroundSize.y + 15, 30, 30 }, GuiIconText(RICON_FILE_DELETE, "")))
		{
			song = song->Next;
			playlist.Remove(songIndex);
			continue;
		}

		songIndex++;
		song = song->Next;
	}
	EndScissorMode();
}

void DrawGUI(MusicPlayer::Player& player, Font& font)
{
	auto currentSong = player.GetPlayingSong();
	auto playlistCount = player.GetPlaylist().Size();
	Vector2 resolution = { (float)GetScreenWidth(), (float)GetScreenHeight() };
	Vector2 center = { resolution.x / 2, resolution.y / 2 };

	// Song name
	GuiDrawText((currentSong ? currentSong->Value.Name : "-").c_str(), { 20, resolution.y - 65, 150.0f, 20 }, 0, RAYWHITE);
	// Artist
	GuiDrawText((currentSong ? currentSong->Value.Artist : "-").c_str(), { 20, resolution.y - 40, 150.0f, 20 }, 0, GRAY);

	// Song progress bar
	// Background
	GuiDrawRectangle({ 20, resolution.y - 80, resolution.x - 40, 5 }, 0, RAYWHITE, { 0, 0, 0, 50 });
	// Foreground
	GuiDrawRectangle({ 20, resolution.y - 80, ((resolution.x - 40) * player.GetTimePlayed() / player.GetTotalSongTime()), 5 }, 0, RAYWHITE, { 171, 115, 255, 255 });

	// Song current time
	DrawTextEx(font, GetTimecode(player.GetTimePlayed()).c_str(), { 20, resolution.y - 100 }, 17, 1, GRAY);
	// Song total time
	DrawTextEx(font, GetTimecode(player.GetTotalSongTime()).c_str(), { resolution.x - 45, resolution.y - 100 }, 17, 1, GRAY);

	float mediaButtonSize = 40.0f;
	// Button previous
	if (GuiButton(
		{
			center.x - mediaButtonSize * 2,
			resolution.y - mediaButtonSize - 20,
			mediaButtonSize,
			mediaButtonSize
		}, GuiIconText(RICON_PLAYER_PREVIOUS, "")))
		player.Previous();
	// Button play/pause
	if (GuiButton(
		{
			center.x - mediaButtonSize / 2,
			resolution.y - mediaButtonSize - 20,
			mediaButtonSize,
			mediaButtonSize
		}, GuiIconText(player.IsPlaying() ? RICON_PLAYER_PAUSE : RICON_PLAYER_PLAY, "")))
		player.TogglePause();
	// Button next
	if (GuiButton(
		{
			center.x + mediaButtonSize,
			resolution.y - mediaButtonSize - 20,
			mediaButtonSize,
			mediaButtonSize
		}, GuiIconText(RICON_PLAYER_NEXT, "")))
		player.Next();

	// Volume
	float playerVolume = player.GetVolume() * 100;
	float volume = GuiSliderBar(
		{
			resolution.x - (resolution.x * 0.2f + 20),
			resolution.y - mediaButtonSize,
			resolution.x * 0.2f - 20,
			10
		},
		"", "",
		playerVolume,
		0,
		100);
	if (volume != playerVolume)
		player.SetVolume(volume / 100.0f);

	if (GuiButton(
		{
			resolution.x - (resolution.x * 0.2f + 60),
			resolution.y - mediaButtonSize - 10,
			30,
			30
		},
		GuiIconText(RICON_AUDIO, "")
	))
		player.ToggleMute();

	DrawPlaylistUI(player, font);
}