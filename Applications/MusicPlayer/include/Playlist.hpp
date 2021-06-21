#pragma once
#include <Song.hpp>
#include <DataStructures/DoubleLinkedList.hpp>

using namespace LCDS;

namespace MusicPlayer
{
	const std::string DefaultPlaylistPath = "./playlist.dat";

	class Playlist
	{
		DoubleLinkedList<Song> m_List;

	public:
		Playlist();
		Playlist(std::string filepath);

		void Add(Song song);
		void Insert(Song song, unsigned int index);

		void Remove(unsigned int index);
		void Remove(std::string songName);

		void Clear();

		void Save(std::string filepath = DefaultPlaylistPath);
		void Load(std::string filepath = DefaultPlaylistPath);

		Song GetSong(unsigned int index);
		DoubleLinkedListNode<Song>* Get(unsigned int index);
		DoubleLinkedListNode<Song>* GetFirst();
		DoubleLinkedListNode<Song>* GetLast();

		unsigned int Size() const;

		DoubleLinkedListNode<Song>* operator [](unsigned int index) { return Get(index); }
	};
}