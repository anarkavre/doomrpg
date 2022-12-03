#include <stdexcept>

#include "SDL_image.h"

#include "CWindow.h"
#include "Game/CGame.h"

CWindow::~CWindow()
{
	if (m_window != nullptr)
		SDL_DestroyWindow(m_window);
}

bool CWindow::Initialize(CGame *game, const char *title, int x, int y, int w, int h, Uint32 flags, const char *icon)
{
	if (!m_initialized && game != nullptr)
	{
		m_game = game;

		CConsole &console = m_game->GetConsole();

		if (flags & SDL_WINDOW_OPENGL)
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
			SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
		}

		console.Write("Creating window...");

		if ((m_window = SDL_CreateWindow(title, x, y, w, h, flags)) == nullptr)
			throw std::runtime_error("Could not create window");

		console.Write("OK\n");

		m_id = SDL_GetWindowID(m_window);
		m_width = w;
		m_height = h;

		if (icon != nullptr)
		{
			SDL_Surface *surface = IMG_Load(icon);
			SDL_SetWindowIcon(m_window, surface);
			SDL_FreeSurface(surface);
		}

		m_initialized = true;
	}

	return m_initialized;
}