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
#include <DataStructures/DoubleLinkedList.hpp>

namespace MusicPlayer
{
	const unsigned int SongStringLength = 16; // Max length of 'Name' and 'Artist' when saving to file
	const unsigned int FilepathStringLength = 64;

	struct Song
	{
		std::string Name;
		std::string Artist;
		std::string Filepath;

		bool operator >(Song& other) { return strcmp(Name.c_str(), other.Name.c_str()) < 0; }
	};
}