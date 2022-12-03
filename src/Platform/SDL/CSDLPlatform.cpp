#include <stdexcept>

#include "SDL_image.h"
#include "SDL_mixer.h"

#include "Game/CGame.h"
#include "Platform/SDL/CSDLPlatform.h"
#include "Window/CWindow.h"

CSDLPlatform::~CSDLPlatform()
{
	if (m_initialized)
	{
		Mix_CloseAudio();

		if (m_window != nullptr)
			delete m_window;

		Mix_Quit();

		IMG_Quit();

		SDL_Quit();
	}
}

bool CSDLPlatform::Initialize(CGame *game)
{
	if (!m_initialized && game != nullptr)
	{
		m_game = game;

		CConsole &console = m_game->GetConsole();

		console.Write("Initializing SDL...");

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
			throw std::runtime_error("Could not initialize SDL");

		console.Write("OK\n");

		console.Write("Initializing SDL_image...");

		if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
			throw std::runtime_error("Could not initialize SDL_image");

		console.Write("OK\n");

		console.Write("Initializing SDL_mixer...");

		if ((Mix_Init(MIX_INIT_MP3) & MIX_INIT_MP3) != MIX_INIT_MP3)
			throw std::runtime_error("Could not initialize SDL_mixer");

		console.Write("OK\n");

		const char *title = m_game->GetTitle().c_str();
		int x = SDL_WINDOWPOS_CENTERED, y = SDL_WINDOWPOS_CENTERED, w = 640, h = 480;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_GRABBED;
		const char *icon = m_game->GetIcon().c_str();

		m_window = new CWindow(m_game, title, x, y, w, h, flags, icon);

		console.Write("Opening audio...");

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
			throw std::runtime_error("Could not open audio");

		console.Write("OK\n");

		m_initialized = true;
	}

	return m_initialized;
}

unsigned int CSDLPlatform::GetTicks()
{
	return SDL_GetTicks();
}