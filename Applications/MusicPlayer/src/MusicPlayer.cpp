#include <MusicPlayer.hpp>
#include <raylib.h>

using namespace std;
using namespace LCDS;
using namespace MusicPlayer;

Player::Player() : m_State(MusicState::Stopped), m_Playlist(), m_CurrentSong(nullptr) { }

Player::~Player() { Stop(); }

void Player::Update()
{
	if(m_State == MusicState::Playing)
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

	m_CurrentMusic = LoadMusicStream(m_CurrentSong->Value.Filepath.c_str());
	PlayMusicStream(m_CurrentMusic);
}

void Player::Play()
{
	if(m_State == MusicState::Stopped)
		return Play(0);
	ResumeMusicStream(m_CurrentMusic);
}

void Player::Play(unsigned int index)
{
	if (m_State != MusicState::Stopped)
		StopMusicStream(m_CurrentMusic);

	m_CurrentSong = m_Playlist.Get(index);
	if (m_CurrentSong->Value.Filepath.empty())
	{
		cerr << "Tried to play song '" << m_CurrentSong->Value.Name << "' but no filepath given" << endl;
		return;
	}
	PlayCurrentSong();
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
	if (m_State != MusicState::Paused)
		return; // Can only resume what is paused
	m_State = MusicState::Playing;
	ResumeMusicStream(m_CurrentMusic);
}

void Player::TogglePause()
{
	if (m_State == MusicState::Paused)
		Resume();
	else if (m_State == MusicState::Playing)
		Pause();
}

void Player::Next()
{
	if (!m_CurrentSong->Next)
		return; // No song in queue/playlist
	m_CurrentSong = m_CurrentSong->Next;
	PlayCurrentSong();
}

void Player::Previous()
{
	if (!m_CurrentSong->Previous)
		return; // No previous song
	m_CurrentSong = m_CurrentSong->Previous;
	PlayCurrentSong();
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
	if (m_State == MusicState::Stopped)
		return nullptr;
	return m_CurrentSong;
}