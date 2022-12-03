#ifndef __CINPUTSYSTEM_H__
#define __CINPUTSYSTEM_H__

#include <unordered_map>

#include "SDL.h"

#include "Input/CInputAction.h"
#include "Interface/IEventHandler.h"
#include "Interface/ISystem.h"

class CInputSystem : public ISystem, public IEventHandler
{
public:
	CInputSystem(CGame *game = nullptr) : m_gameController(nullptr) { Initialize(game); }
	~CInputSystem();

	bool Initialize(CGame *game);
	bool ProcessEvent(const SDL_Event &event);
	CInputAction &RegisterAction(const std::string &name);
	void UnregisterAction(const std::string &name);
	void BindAction(const std::string &name, SDL_Keycode sym);
	void UnbindAction(SDL_Keycode sym);

	CInputAction *GetAction(const std::string &name) { return (m_actions.count(name) == 1 ? &m_actions.at(name) : nullptr); }

private:
	std::unordered_map<std::string, CInputAction> m_actions;
	std::unordered_map<SDL_Keycode, CInputAction *> m_bindings;
	SDL_GameController *m_gameController;
};

#endif