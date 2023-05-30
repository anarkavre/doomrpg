#ifndef __CGAME_H__
#define __CGAME_H__

#include <memory>
#include <string>

#include "CGameDefaults.h"
#include "Config/CConfig.h"
#include "Console/CConsole.h"
#include "Game/CGameLoop.h"
#include "Game/CGameState.h"
#include "Logger/CLogger.h"
#include "Manager/CGeometryManager.h"
#include "Manager/CMaterialManager.h"
#include "Manager/CMeshManager.h"
#include "Manager/CResourceManager.h"
#include "Platform/SDL/CSDLPlatform.h"
#include "System/CAnimationSystem.h"
#include "System/CCameraSystem.h"
#include "System/CInputSystem.h"
#include "System/CRenderSystem.h"
#include "System/CSceneSystem.h"
#include "System/CUISystem.h"
#include "Renderer/OpenGL/COpenGLRenderer.h"
#include "Resource/CTexture.h"
#include "Interface/IPlatform.h"
#include "Interface/IRenderer.h"
#include "Interface/IRenderHandler.h"

class CFont;

class CGame : public ISystem
{
public:
	CGame() {}
	CGame(const std::string &title, const std::string &icon, const std::string &log, const std::string &config, int argc = 0, char *argv[] = nullptr) { Initialize(title, icon, log, config, argc, argv); }
	~CGame();

	bool Initialize(const std::string &title = "Game", const std::string &icon = "", const std::string &log = "game.log", const std::string &config = "game.cfg", int argc = 0, char *argv[] = nullptr);
	void Run();
	bool Update() { if (m_gameState != nullptr) m_gameState->Update(); return false; }

	const std::string &GetTitle() const { return m_title; }
	void SetTitle(const std::string &title) { m_title = title; }
	const std::string &GetIcon() const { return m_icon; }
	void SetIcon(const std::string &icon) { m_icon = icon; }
	bool GetRunning() const { return m_gameLoop.GetRunning(); }
	void SetRunning(bool running) { m_gameLoop.SetRunning(running); }
	CLogger &GetLogger() { return m_logger; }
	CConfig &GetConfig() { return m_config; }
	CGameLoop &GetGameLoop() { return m_gameLoop; }
	CGameState *GetGameState() const { return m_gameState; }
	void SetGameState(CGameState *gameState) { m_gameState = gameState; }
	IPlatform *GetPlatform() { return m_platform; }
	IRenderer *GetRenderer() { return m_renderer; }
	CResourceManager &GetResourceManager() { return m_resourceManager; }
	CMaterialManager &GetMaterialManager() { return m_materialManager; }
	CGeometryManager &GetGeometryManager() { return m_geometryManager; }
	CMeshManager &GetMeshManager() { return m_meshManager; }
	CInputSystem &GetInputSystem() { return m_inputSystem; }
	CAnimationSystem &GetAnimationSystem() { return m_animationSystem; }
	CCameraSystem &GetCameraSystem() { return m_cameraSystem; }
	CSceneSystem &GetSceneSystem() { return m_sceneSystem; }
	CUISystem &GetUISystem() { return m_uiSystem; }
	CRenderSystem &GetRenderSystem() { return m_renderSystem; }
	CConsole &GetConsole() { return m_console; }
	std::shared_ptr<CFont> GetFont() const { return m_font; }
	CWindow &GetWindow() { return m_sdlPlatform.GetWindow(); }
	CTimer &GetTimer() { return m_gameLoop.GetTimer(); }

	bool IsInitialized() const { return m_initialized; }
	bool IsRunning() const { return m_gameLoop.IsRunning(); }

private:
	bool m_initialized{false};
	std::string m_title;
	std::string m_icon;
	CLogger m_logger;
	CConfig m_config;
	CGameLoop m_gameLoop;
	CGameState *m_gameState{nullptr};
	CSDLPlatform m_sdlPlatform;
	IPlatform *m_platform{nullptr};
	COpenGLRenderer m_glRenderer;
	IRenderer *m_renderer{nullptr};
	CResourceManager m_resourceManager;
	CMaterialManager m_materialManager;
	CGeometryManager m_geometryManager;
	CMeshManager m_meshManager;
	CRenderSystem m_renderSystem;
	CUISystem m_uiSystem;
	CSceneSystem m_sceneSystem;
	CCameraSystem m_cameraSystem;
	CAnimationSystem m_animationSystem;
	CInputSystem m_inputSystem;
	CConsole m_console;
	std::shared_ptr<CFont> m_font;
};

#endif
