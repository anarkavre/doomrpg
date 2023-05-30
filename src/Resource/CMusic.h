#ifndef __CMUSIC_H__
#define __CMUSIC_H__

#include "SDL_mixer.h"

#include "CResource.h"

class CMusic : public CResource
{
public:
	CMusic(CGame *game, unsigned int handle, const std::string &name);
	~CMusic();

	void Play();
	void Stop();
	void Pause();
	void Resume();

	bool GetRepeat() const { return m_repeat; }
	void SetRepeat(bool repeat) { m_repeat = repeat; }

	static const std::string &GetResourcePath() { return m_resourcePath; }

private:
	Mix_Music *m_music{nullptr};
	bool m_repeat{false};

	static const std::string m_resourcePath;
};

#endif