#include <stdexcept>

#include "CMusic.h"
#include "Game/CGame.h"

CMusic::CMusic(CGame *game, unsigned int handle, const std::string &name) : CResource(game, handle, name, GetResourcePath(), ECResourceType::Music)
{
	unsigned char *data = new unsigned char[GetSize()];
	GetData(data, GetSize());

	SDL_RWops *rw = SDL_RWFromMem(data, GetSize());

	m_music = Mix_LoadMUS_RW(rw, 1);

	delete[] data;
}

CMusic::~CMusic()
{
	if (m_music != nullptr)
		Mix_FreeMusic(m_music);
}

void CMusic::Play()
{
	Mix_PlayMusic(m_music, (m_repeat ? -1 : 1));
}

void CMusic::Stop()
{
	Mix_HaltMusic();
}

void CMusic::Pause()
{
	Mix_PauseMusic();
}

void CMusic::Resume()
{
	Mix_ResumeMusic();
}

const std::string CMusic::m_resourcePath = "music/";