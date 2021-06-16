#include <string>
#include <fstream>
#include <algorithm>
#include <Playlist.hpp>

using namespace std;
using namespace LCDS;
using namespace MusicPlayer;

Playlist::Playlist() { }
Playlist::Playlist(string filepath) { Load(filepath); }

unsigned int Playlist::Size() const { return m_List.Size(); }

void Playlist::Add(Song& song) { m_List.Add(song); }
void Playlist::Insert(Song& song, unsigned int index) { m_List.Insert(song, index); }

void Playlist::Remove(unsigned int index) { m_List.RemoveAt(index); }
void Playlist::Remove(string songName)
{
	transform(songName.begin(), songName.end(), songName.begin(), tolower);
	auto node = m_List.Find([=](Song& s)
		{
			// Create temporary song name, and convert to lowercase for testing
			string tempName = s.Name;
			transform(tempName.begin(), tempName.end(), tempName.begin(), tolower);
			return songName.compare(tempName) == 0;
		});
	if (!node)
		return; // Couldn't find song
	m_List.Remove(node);
}

void Playlist::Clear() { m_List.Clear(); }

void Playlist::Save(string filepath)
{
	ofstream output(filepath, ios::in | ios::binary);

	output.write((char*)m_List.Size(), sizeof(unsigned int));

	auto node = m_List.GetHead();
	while (node)
	{
		output.write(node->Value.Name.c_str(), SongStringLength);
		output.write(node->Value.Artist.c_str(), SongStringLength);
		node = node->Next;
	}

	output.flush();
	output.close();
}

void Playlist::Load(string filepath)
{
	Clear();

	ifstream input(filepath, ios::in | ios::binary);
	char* temp = nullptr;

	if (!input)
		return; // File could not be opened

	input.read(temp, sizeof(unsigned int));
	unsigned int count = (unsigned int)temp;

	for (int i = 0; i < count; i++)
	{
		Song song;

		input.read(temp, SongStringLength);
		song.Name = string(temp, SongStringLength);

		input.read(temp, SongStringLength);
		song.Artist = string(temp, SongStringLength);

		Add(song);
	}

	input.close();
}

Song Playlist::GetSong(unsigned int index)
{
	auto node = Get(index);
	return node ? node->Value : Song();
}

DoubleLinkedListNode<Song>* Playlist::GetLast() { return m_List.GetTail(); }
DoubleLinkedListNode<Song>* Playlist::GetFirst() { return m_List.GetHead(); }
DoubleLinkedListNode<Song>* Playlist::Get(unsigned int index) { return m_List.GetNode(index); }