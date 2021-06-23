/*
 *
 * AIE Introduction to C++
 * Playlist Viewer - Demonstration of double linked lists
 * Lewis Comstive (s210314)
 *
 * See the LICENSE file in the root directory of project for copyright.
 *
 */
#include <MusicPlayer.hpp>

using namespace std;
using namespace LCDS;
using namespace MusicPlayer;

// Custom string hashing function
unsigned int FilepathHash(string& filepath)
{
	const unsigned int RandomNumbers[] =
	{
		601,
		217,
		679,
		662,
		845,
		711,
		453,
		358,
		909,
		423,
	};
	const unsigned int RandomNumberCount = 10;

	unsigned int hash = RandomNumbers[RandomNumberCount / 2];
	for (int i = 0; i < filepath.size(); i++)
		hash += filepath[i] << hash ^ RandomNumbers[(i | hash) % RandomNumberCount];

		return hash;
}

Player::Player() : m_State(MusicState::Stopped), m_CurrentMusic(), m_Playlist(), m_CurrentSong(nullptr)
{
	SetVolume(1.0f);

	// If not created as a pointer, causes access violation upon deletion of values array (in this case, Raylib's Music)
	m_MusicCache = make_unique<HashTable<string, Music>>(25, FilepathHash);
}

Player::~Player() { Stop(); }

void Player::Update()
{
	if (m_State == MusicState::Playing)
		UpdateMusicStream(m_CurrentMusic);
}

void Player::Stop()
{
	if (!m_CurrentSong)
		return;
	StopMusicStream(m_CurrentMusic);

	m_CurrentMusic = {};
	m_CurrentSong = nullptr;
	m_State = MusicState::Stopped;
}

void Player::PlayCurrentSong()
{
	m_State = MusicState::Playing;
	PlayMusicStream(m_CurrentMusic);
}

void Player::Play()
{
	if (m_State == MusicState::Stopped)
		return Play(0);
	Resume();
}

void Player::Play(unsigned int index)
{
	Load(index);
	PlayCurrentSong();
}

void Player::Load(unsigned int index)
{
	if (m_State != MusicState::Stopped)
		StopMusicStream(m_CurrentMusic);
	Load(m_Playlist.Get(index));
}

void Player::Load(DoubleLinkedListNode<Song>* song)
{
	if (!song)
		return;
	if (song->Value.Filepath.empty())
	{
		cerr << "Tried to load song '" << song->Value.Name << "' but no filepath given" << endl;
		return;
	}

	m_CurrentSong = song;

	string& filepath = song->Value.Filepath;
	if (!m_MusicCache->Contains(filepath)) // Add song to cache
		m_MusicCache->Add(filepath, LoadMusicStream(filepath.c_str()));

	// Load song from cache
	m_CurrentMusic = m_MusicCache->Find(filepath);
	m_State = MusicState::Loaded;
}

void Player::Pause()
{
	if (m_State != MusicState::Playing)
		return; // No need to pause
	m_State = MusicState::Paused;
	PauseMusicStream(m_CurrentMusic);
}

void Player::Resume()
{
	switch (m_State)
	{
	case MusicState::Paused:
		ResumeMusicStream(m_CurrentMusic);
		break;
	case MusicState::Loaded:
		PlayMusicStream(m_CurrentMusic);
		break;
	default:
		return;
	}

	m_State = MusicState::Playing;
}

void Player::TogglePause()
{
	if (m_State == MusicState::Playing)
		Pause();
	else
		Resume();
}

void Player::Next()
{
	if (!m_CurrentSong || !m_CurrentSong->Next)
		return; // No song in queue/playlist
	Load(m_CurrentSong->Next);
	StopMusicStream(m_CurrentMusic); // Reset play position
	PlayCurrentSong();
}

void Player::Previous()
{
	if (!m_CurrentSong || !m_CurrentSong->Previous)
		return; // No previous song
	Load(m_CurrentSong->Previous);
	StopMusicStream(m_CurrentMusic); // Reset play position
	PlayCurrentSong();
}

void Player::Seek(int position)
{
	/// rAudioBuffer is an "incomplete type", although defined in raudio.c
	/// Would have thought building against raylib.lib would have fixed?
	/// Can't find another way to seek Raylib raudio streams?

	/*
	struct rAudioBuffer* buffer = m_CurrentMusic.stream.buffer;
	int maxSamples = buffer->sampleCount;
	if (position > buffer->sampleCount)
		position = buffer->sampleCount - 1; // Play last frame
	m_CurrentMusic.stream.buffer->frameCursorPos = position;
	*/
}

bool Player::IsPlaying() { return m_State == MusicState::Playing; }

MusicState& Player::GetState() { return m_State; }

Playlist& Player::GetPlaylist() { return m_Playlist; }

float Player::GetTimePlayed() { return m_State == MusicState::Stopped ? 0 : GetMusicTimePlayed(m_CurrentMusic); }
float Player::GetTotalSongTime() { return m_State == MusicState::Stopped ? 0 : GetMusicTimeLength(m_CurrentMusic); }

void Player::LoadPlaylist(std::string filepath)
{
	Stop();
	m_Playlist.Load(filepath);
}

DoubleLinkedListNode<Song>* Player::GetPlayingSong()
{
	if (m_State == MusicState::Stopped || m_Playlist.Size() == 0)
		return (m_CurrentSong = nullptr);
	return m_CurrentSong;
}

void Player::SetVolume(float value)
{
	m_Volume = value;
	if (m_Volume < 0.001f) m_Volume = 0.0f;
	if (m_Volume > 1.0f) m_Volume = 1.0f;
	SetMusicVolume(m_CurrentMusic, m_Volume);
	m_Muted = value > 0.001f;
}

float Player::GetVolume() { return m_Volume; }

void Player::Mute(bool mute)
{
	SetMusicVolume(m_CurrentMusic, mute ? 0.0f : m_Volume);
	m_Muted = mute;
}

void Player::ToggleMute() { Mute(!m_Muted); }

bool Player::GetMuted() { return m_Muted; }