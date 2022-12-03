#include <stdexcept>

#include "Game/CGame.h"
#include "Resource/CFont.h"
#include "Window/CWindow.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

CGame::~CGame()
{
	m_console.SaveVariables(m_config);
}

bool CGame::Initialize(const std::string &title, const std::string &icon, const std::string &log, const std::string &config, int argc, char *argv[])
{
	if (!m_initialized)
	{
		try
		{
			m_title = title;
			m_icon = icon;

			m_logger.Initialize(this, log);

			m_config.Initialize(this, config);

			char *dataPath = nullptr;

			if (argc > 1)
			{
				for (int i = 1; i < argc; i++)
				{
					if (!strcmp(argv[i], "-data"))
						dataPath = argv[i + 1];
				}
			}

			m_sdlPlatform.Initialize(this);
			m_platform = &m_sdlPlatform;

			m_glRenderer.Initialize(this, &m_sdlPlatform.GetWindow());
			m_glRenderer.SetViewport(0, 0, m_sdlPlatform.GetWindow().GetWidth(), m_sdlPlatform.GetWindow().GetHeight());
			m_glRenderer.SetOrthoMatrix(0.0f, static_cast<float>(m_sdlPlatform.GetWindow().GetWidth()), static_cast<float>(m_sdlPlatform.GetWindow().GetHeight()), 0.0f);
			m_glRenderer.SetDepthTest(true);
			m_glRenderer.SetVsync(true);
			m_renderer = &m_glRenderer;

			m_resourceManager.Initialize(this);

			if (dataPath != nullptr)
				m_resourceManager.SetBasePath(dataPath);

			m_materialManager.Initialize(this);

			m_geometryManager.Initialize(this);

			m_meshManager.Initialize(this);

			m_inputSystem.Initialize(this);

			m_animationSystem.Initialize(this);

			m_cameraSystem.Initialize(this);

			m_gameLoop.RegisterSystem(*this);

			m_sceneSystem.Initialize(this);
			
			m_uiSystem.Initialize(this);

			m_font = m_resourceManager.AcquireResource<CFont>(defaultGameFont);
			m_console.Resize(m_sdlPlatform.GetWindow().GetWidth() / (m_font->GetCellWidth() - ((m_font->GetCellWidth() % 2) == 0 ? 0 : 1)), m_sdlPlatform.GetWindow().GetHeight() / (2 * m_font->GetCellHeight()));
			m_console.Initialize(this);

			m_renderSystem.Initialize(this);

			m_console.RegisterCommand("quit", [this](CConsole &console, const std::vector<std::string> &arguments) { SetRunning(false); });
			m_console.RegisterVariable("fullscreen", m_sdlPlatform.GetWindow().GetFullscreen(), [this](bool &value) { m_sdlPlatform.GetWindow().SetFullscreen(value); });
			m_console.RegisterVariable("vsync", m_glRenderer.GetVsync(), [this](bool &value) { static_cast<COpenGLRenderer *>(m_renderer)->SetVsync(value); });
			m_console.LoadVariables(m_config);

			m_gameLoop.Initialize(this);

			SDL_StopTextInput();
			SDL_SetRelativeMouseMode(SDL_TRUE);

			m_initialized = true;
		}
		catch (const std::exception &e)
		{
			m_logger.Write(std::string(e.what()) + "\n");

			throw;
		}
	}

	return m_initialized;
}

void CGame::Run()
{
	if (!m_initialized)
		return;

	m_gameLoop.Run();
}