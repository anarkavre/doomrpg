#ifndef __CSDLPLATFORM_H__
#define __CSDLPLATFORM_H__

#include "SDL.h"

#include "Interface/IPlatform.h"

class CGame;
class CWindow;

class CSDLPlatform : public IPlatform
{
public:
	CSDLPlatform() : IPlatform(ECPlatformType::SDL, "SDL"), m_initialized(false) {}
	CSDLPlatform(CGame *game) : IPlatform(ECPlatformType::SDL, "SDL"), m_initialized(false) { Initialize(game); }
	~CSDLPlatform();

	bool Initialize(CGame *game);
	unsigned int GetTicks();

	CWindow &GetWindow() { return *m_window; }

private:
	bool m_initialized;
	CGame *m_game;
	CWindow *m_window;
};

#endif