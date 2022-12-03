#ifndef __CWINDOW_H__
#define __CWINDOW_H__

#include "SDL.h"

class CGame;

class CWindow
{
public:
	CWindow() : m_initialized(false) {}
	CWindow(CGame *game, const char *title, int x, int y, int w, int h, Uint32 flags, const char *icon = nullptr) : m_initialized(false), m_fullscreen(false) { Initialize(game, title, x, y, w, h, flags, icon); }
	~CWindow();

	bool Initialize(CGame *game, const char *title, int x, int y, int w, int h, Uint32 flags, const char *icon = nullptr);

	SDL_Window *GetWindow() const { return m_window; }
	Uint32 GetID() const { return m_id; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	bool GetFullscreen() const { return m_fullscreen; }
	void SetFullscreen(bool fullscreen) { m_fullscreen = fullscreen; SDL_SetWindowFullscreen(m_window, (m_fullscreen ? SDL_WINDOW_FULLSCREEN : 0)); }

	bool IsInitialized() const { return m_initialized; }

private:
	bool m_initialized;
	CGame *m_game;
	SDL_Window *m_window;
	Uint32 m_id;
	int m_width, m_height;
	bool m_fullscreen;
};

#endif