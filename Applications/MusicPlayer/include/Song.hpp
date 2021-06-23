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
	};
}