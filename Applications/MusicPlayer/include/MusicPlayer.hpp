#pragma once
#include <raylib.h>
#include <Playlist.hpp>

using namespace LCDS; // Lewis Comstive's Data Structures

namespace MusicPlayer
{
	enum class MusicState { Stopped, Paused, Playing };

	class Player
	{
		MusicState m_State;
		Playlist m_Playlist;
		Music m_CurrentMusic; // Raylib music
		DoubleLinkedListNode<Song>* m_CurrentSong;

		void PlayCurrentSong();

	public:
		Player();
		~Player();

		void Update();

		void Stop();
		void Pause();
		void Resume();
		void TogglePause();

		void Play();
		void Play(unsigned int index);

		void Next();
		void Previous();

		bool IsPlaying();

		DoubleLinkedListNode<Song>* GetPlayingSong();

		MusicState& GetState();
		Playlist& GetPlaylist();
		void LoadPlaylist(std::string filepath);

		float GetTimePlayed();
		float GetTotalSongTime();
	};
}