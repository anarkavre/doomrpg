#include <stdexcept>

#include "CSound.h"
#include "Game/CGame.h"

CSound::CSound(CGame *game, unsigned int handle, const std::string &name) : CResource(game, handle, name, GetResourcePath(), ECResourceType::Sound)
{
	unsigned char *data = new unsigned char[GetSize()];
	GetData(data, GetSize());

	SDL_RWops *rw = SDL_RWFromMem(data, GetSize());

	m_chunk = Mix_LoadWAV_RW(rw, 1);

	delete[] data;
}

CSound::~CSound()
{
	if (m_chunk != nullptr)
		Mix_FreeChunk(m_chunk);
}

void CSound::Play()
{
	Mix_PlayChannel(-1, m_chunk, 0);
}

const std::string CSound::m_resourcePath = "sounds/";