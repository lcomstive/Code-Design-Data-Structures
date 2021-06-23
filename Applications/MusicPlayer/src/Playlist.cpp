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

void Playlist::Add(Song song) { m_List.Add(song); }
void Playlist::Insert(Song song, unsigned int index) { m_List.Insert(song, index); }

void Playlist::Shift(unsigned int index, bool forward) { m_List.Shift(index, forward); }

void Playlist::Remove(unsigned int index) { m_List.RemoveAt(index); }
void Playlist::Remove(string songName)
{
	transform(songName.begin(), songName.end(), songName.begin(), ::tolower);
	auto node = m_List.Find([=](Song& s)
		{
			// Create temporary song name, and convert to lowercase for testing
			string tempName = s.Name;
			transform(tempName.begin(), tempName.end(), tempName.begin(), ::tolower);
			return songName.compare(tempName) == 0;
		});
	if (!node)
		return; // Couldn't find song
	m_List.Remove(node);
}

void Playlist::Clear() { m_List.Clear(); }

void FillCharArray(string& input, char* output, size_t maxLength)
{
	size_t stringLength = input.size();
	// Copy name into tempStr
	memcpy(output, input.data(), min(maxLength, stringLength));
	// Pad remainder of string with null-terminators
	if (stringLength < maxLength)
		memset(output + stringLength, '\0', maxLength - stringLength);
}

void Playlist::Save(string filepath)
{
	if (m_List.IsEmpty())
		return;

	ofstream output(filepath, ios::out | ios::binary);

	unsigned int listSize = m_List.Size();
	output.write((char*)&listSize, sizeof(unsigned int));

	auto node = m_List.GetHead();
	char tempStr[max(SongStringLength, FilepathStringLength)];
	while (node)
	{
		FillCharArray(node->Value.Name, tempStr, SongStringLength);
		output.write(tempStr, SongStringLength);

		FillCharArray(node->Value.Artist, tempStr, SongStringLength);
		output.write(tempStr, SongStringLength);

		FillCharArray(node->Value.Filepath, tempStr, FilepathStringLength);
		output.write(tempStr, FilepathStringLength);

		node = node->Next;
	}

	output.flush();
	output.close();
	cout << "Saved playlist to '" << filepath << "'" << endl;
}

void Playlist::Load(string filepath)
{
	Clear();

	ifstream input(filepath, ios::in | ios::binary);
	char temp[max(SongStringLength, FilepathStringLength)];

	if (!input)
	{
		cout << "Tried loading playlist '" << filepath << "', but file was not found" << endl;
		return; // File could not be opened
	}

	input.read(temp, sizeof(unsigned int));
	unsigned int count = 0;
	memcpy(&count, temp, sizeof(unsigned int)); // Fill `count` with `temp`

	for (unsigned int i = 0; i < count; i++)
	{
		Song song;

		input.read(temp, SongStringLength);
		song.Name = string(temp, SongStringLength);

		input.read(temp, SongStringLength);
		song.Artist = string(temp, SongStringLength);

		input.read(temp, FilepathStringLength);
		song.Filepath = string(temp, FilepathStringLength);

		Add(song);
	}

	input.close();
	cout << "Loaded playlist '" << filepath << "'" << endl;
}

Song Playlist::GetSong(unsigned int index)
{
	auto node = Get(index);
	return node ? node->Value : Song();
}

DoubleLinkedListNode<Song>* Playlist::GetLast() { return m_List.GetTail(); }
DoubleLinkedListNode<Song>* Playlist::GetFirst() { return m_List.GetHead(); }
DoubleLinkedListNode<Song>* Playlist::Get(unsigned int index) { return m_List.GetNode(index); }