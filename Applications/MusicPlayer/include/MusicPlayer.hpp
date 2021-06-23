/*
 *
 * AIE Introduction to C++
 * Playlist Viewer - Demonstration of double linked lists
 * Lewis Comstive (s210314)
 *
 * See the LICENSE file in the root directory of project for copyright.
 *
 */
#pragma once
#include <raylib.h>
#include <Playlist.hpp>
#include <DataStructures/HashTable.hpp>

using namespace LCDS; // Lewis Comstive's Data Structures

namespace MusicPlayer
{
	enum class MusicState { Stopped, Loaded, Paused, Playing };

	class Player
	{
		bool m_Muted;
		float m_Volume;
		MusicState m_State;
		Playlist m_Playlist;
		Music m_CurrentMusic; // Raylib music
		std::unique_ptr<HashTable<std::string, Music>> m_MusicCache; // Cached raylib music
		DoubleLinkedListNode<Song>* m_CurrentSong;

		void Load(DoubleLinkedListNode<Song>* song);
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
		void Load(unsigned int index = 0);

		void Next();
		void Previous();

		// Note: Doesn't work, see definition for details
		void Seek(int position);

		bool IsPlaying();

		void SetVolume(float volume);
		float GetVolume();
		bool GetMuted();
		void Mute(bool mute = true);
		void ToggleMute();

		DoubleLinkedListNode<Song>* GetPlayingSong();

		MusicState& GetState();
		Playlist& GetPlaylist();
		void LoadPlaylist(std::string filepath);

		float GetTimePlayed();
		float GetTotalSongTime();
	};
}