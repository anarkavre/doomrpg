#include "CInputSystem.h"
#include "Game/CGame.h"
#include "Window/CWindow.h"

CInputSystem::~CInputSystem()
{
	if (m_initialized)
	{
		m_game->GetGameLoop().UnregisterEventHandler(*this);

		if (m_gameController != nullptr)
			SDL_GameControllerClose(m_gameController);
	}
}

bool CInputSystem::Initialize(CGame *game)
{
	if (ISystem::Initialize(game))
	{
		m_game->GetGameLoop().RegisterEventHandler(*this, m_game->GetWindow().GetID());

		m_initialized = true;
	}

	return m_initialized;
}

bool CInputSystem::ProcessEvent(const SDL_Event &event)
{
	if (SDL_IsTextInputActive())
		return false;

	switch (event.type)
	{
	case SDL_KEYDOWN:
		if (m_bindings.count(event.key.keysym.sym) == 1 && !event.key.repeat)
		{
			if (!SDL_IsTextInputActive())
				m_bindings[event.key.keysym.sym]->SetPressed(true);
			else
				m_bindings[event.key.keysym.sym]->SetPressed(false);
		}
		break;
	case SDL_KEYUP:
		if (m_bindings.count(event.key.keysym.sym) == 1)
			m_bindings[event.key.keysym.sym]->SetPressed(false);
		break;
	case SDL_CONTROLLERDEVICEADDED:
	{
		const char *name = SDL_GameControllerNameForIndex(0);

		if (name != nullptr)
			m_game->GetConsole().Write(std::string(name) + " detected.\n");
		else
			m_game->GetConsole().Write("Unknown controller detected.\n");

		m_gameController = SDL_GameControllerOpen(event.cdevice.which);
	}
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		SDL_GameControllerClose(m_gameController);
		m_gameController = nullptr;
		break;
	default:
		break;
	}

	return false;
}

CInputAction &CInputSystem::RegisterAction(const std::string &name)
{
	m_actions.insert(std::make_pair(name, CInputAction(name)));
	return m_actions[name];
}

void CInputSystem::UnregisterAction(const std::string &name)
{
	m_actions.erase(name);
}

void CInputSystem::BindAction(const std::string &name, SDL_Keycode sym)
{
	if (m_actions.count(name) == 1)
		m_bindings.insert(std::make_pair(sym, &m_actions[name]));
}

void CInputSystem::UnbindAction(SDL_Keycode sym)
{
	m_bindings.erase(sym);
}