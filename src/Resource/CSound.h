#ifndef __CSOUND_H__
#define __CSOUND_H__

#include "SDL_mixer.h"

#include "CResource.h"

class CSound : public CResource
{
public:
	CSound(CGame *game, unsigned int handle, const std::string &name);
	~CSound();

	void Play();

	static const std::string &GetResourcePath() { return m_resourcePath; }

private:
	Mix_Chunk *m_chunk;

	static const std::string m_resourcePath;
};

#endif