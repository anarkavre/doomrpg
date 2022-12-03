#include <algorithm>

#include "CGame.h"
#include "CGameLoop.h"

bool CGameLoop::Initialize(CGame *game)
{
	if (game != nullptr)
	{
		m_timer.Initialize(game->GetPlatform());

		m_initialized = true;
	}

	return m_initialized;
}

void CGameLoop::Run()
{
	m_running = true;

	m_accumulator = 0.0f;

	while (m_running)
	{
		ProcessEvents();
		Update();
		Render();
		Present();
	}
}

void CGameLoop::ProcessEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		Uint32 windowId;

		switch (event.type)
		{
		case SDL_QUIT:
			m_running = false;
			windowId = 0;
			break;
		case SDL_WINDOWEVENT:
			windowId = event.window.windowID;
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			windowId = event.key.windowID;
			break;
		case SDL_TEXTEDITING:
			windowId = event.edit.windowID;
			break;
		case SDL_TEXTINPUT:
			windowId = event.text.windowID;
			break;
		case SDL_MOUSEMOTION:
			windowId = event.motion.windowID;
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			windowId = event.button.windowID;
			break;
		case SDL_MOUSEWHEEL:
			windowId = event.wheel.windowID;
			break;
		case SDL_USEREVENT:
			windowId = event.user.windowID;
		default:
			windowId = 0;
			break;
		}

		std::list<IEventHandler *> &eventHandlers = m_eventHandlers[windowId];

		bool unregister = false;

		for (auto it = eventHandlers.begin(); it != eventHandlers.end(); unregister ? it = eventHandlers.erase(it) : ++it)
			unregister = (*it)->ProcessEvent(event);
	}
}

void CGameLoop::Update()
{
	m_timer.Update(m_frameRate);

	float elapsedTime = m_timer.GetElapsedTime();

	m_accumulator += elapsedTime;

	while (m_accumulator >= m_updateInterval)
	{
		bool unregister;

		for (auto it = m_updateHandlers.begin(); it != m_updateHandlers.end(); unregister ? it = m_updateHandlers.erase(it) : ++it)
			unregister = (*it)->Update(m_updateInterval);

		m_accumulator -= m_updateInterval;
	}

	bool unregister;

	for (auto it = m_systems.begin(); it != m_systems.end(); unregister ? it = m_systems.erase(it) : ++it)
		unregister = (*it)->Update();
}

void CGameLoop::Render()
{
	bool unregister;

	for (auto it = m_renderHandlers.begin(); it != m_renderHandlers.end(); unregister ? it = m_renderHandlers.erase(it) : ++it)
		unregister = (*it)->Render(m_accumulator / m_updateInterval);
}

void CGameLoop::Present()
{
	bool unregister;

	for (auto it = m_presentHandlers.begin(); it != m_presentHandlers.end(); unregister ? it = m_presentHandlers.erase(it) : ++it)
		unregister = (*it)->Present();
}

void CGameLoop::RegisterEventHandler(IEventHandler &eventHandler, Uint32 windowId)
{
	std::list<IEventHandler *> &eventHandlers = m_eventHandlers[windowId];

	if (find(eventHandlers.begin(), eventHandlers.end(), &eventHandler) != eventHandlers.end())
		return;

	eventHandlers.push_back(&eventHandler);

	if (windowId != 0)
		RegisterEventHandler(eventHandler);
}

void CGameLoop::RegisterUpdateHandler(IUpdateHandler &updateHandler)
{
	if (find(m_updateHandlers.begin(), m_updateHandlers.end(), &updateHandler) != m_updateHandlers.end())
		return;

	m_updateHandlers.push_back(&updateHandler);
}

void CGameLoop::RegisterSystem(ISystem &system)
{
	if (find(m_systems.begin(), m_systems.end(), &system) != m_systems.end())
		return;

	m_systems.push_back(&system);
}

void CGameLoop::RegisterRenderHandler(IRenderHandler &renderHandler)
{
	if (find(m_renderHandlers.begin(), m_renderHandlers.end(), &renderHandler) != m_renderHandlers.end())
		return;

	m_renderHandlers.push_back(&renderHandler);
}

void CGameLoop::RegisterPresentHandler(IPresentHandler &presentHandler)
{
	if (find(m_presentHandlers.begin(), m_presentHandlers.end(), &presentHandler) != m_presentHandlers.end())
		return;

	m_presentHandlers.push_back(&presentHandler);
}

void CGameLoop::UnregisterEventHandler(IEventHandler &eventHandler, Uint32 windowId)
{
	std::list<IEventHandler *> &eventHandlers = m_eventHandlers[windowId];
	eventHandlers.remove(&eventHandler);

	if (windowId != 0)
		UnregisterEventHandler(eventHandler);
}

void CGameLoop::UnregisterUpdateHandler(IUpdateHandler &updateHandler)
{
	m_updateHandlers.remove(&updateHandler);
}

void CGameLoop::UnregisterSystem(ISystem &system)
{
	m_systems.remove(&system);
}

void CGameLoop::UnregisterRenderHandler(IRenderHandler &renderHandler)
{
	m_renderHandlers.remove(&renderHandler);
}

void CGameLoop::UnregisterPresentHandler(IPresentHandler &presentHandler)
{
	m_presentHandlers.remove(&presentHandler);
}