#ifndef __CGAMELOOP_H__
#define __CGAMELOOP_H__

#include <list>
#include <unordered_map>

#include "Timer/CTimer.h"
#include "Interface/IEventHandler.h"
#include "Interface/IUpdateHandler.h"
#include "Interface/IRenderHandler.h"
#include "Interface/ISystem.h"
#include "Interface/IPresentHandler.h"

class CGame;

class CGameLoop
{
public:
	CGameLoop(CGame *game = nullptr) { Initialize(game); }

	bool Initialize(CGame *game);
	void Run();
	void ProcessEvents();
	void Update();
	void Render();
	void Present();

	void RegisterEventHandler(IEventHandler &eventHandler, Uint32 windowId = 0);
	void RegisterUpdateHandler(IUpdateHandler &updateHandler);
	void RegisterSystem(ISystem &system);
	void RegisterRenderHandler(IRenderHandler &renderHandler);
	void RegisterPresentHandler(IPresentHandler &presentHandler);
	void UnregisterEventHandler(IEventHandler &eventHandler, Uint32 windowId = 0);
	void UnregisterUpdateHandler(IUpdateHandler &updateHandler);
	void UnregisterSystem(ISystem &system);
	void UnregisterRenderHandler(IRenderHandler &renderHandler);
	void UnregisterPresentHandler(IPresentHandler &presentHandler);

	bool GetRunning() const { return m_running; }
	void SetRunning(bool running) { m_running = running; }
	unsigned int GetUpdateRate() const { return m_updateRate; }
	void SetUpdateRate(unsigned int updateRate) { m_updateRate = updateRate; m_updateInterval = 1.0f / m_updateRate; }
	unsigned int GetFrameRate() const { return m_frameRate; }
	void SetFrameRate(unsigned int frameRate) { m_frameRate = frameRate; }
	CTimer &GetTimer() { return m_timer; }

	bool IsInitialized() const { return m_initialized; }
	bool IsRunning() const { return m_running; }

private:
	bool m_initialized{false};
	bool m_running{false};
	unsigned int m_updateRate{100};
	unsigned int m_frameRate{0};
	CTimer m_timer;
	std::unordered_map<Uint32, std::list<IEventHandler *>> m_eventHandlers;
	std::list<IUpdateHandler *> m_updateHandlers;
	std::list<ISystem *> m_systems;
	std::list<IRenderHandler *> m_renderHandlers;
	std::list<IPresentHandler *> m_presentHandlers;
	float m_updateInterval{0.01f};
	float m_accumulator{0.0f};
};

#endif