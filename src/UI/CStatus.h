#ifndef __CSTATUS_H__
#define __CSTATUS_H__

#include <memory>
#include <string>
#include <queue>

#include "Interface/IUpdateHandler.h"
#include "UI/UIElement.h"

class CDoomRPG;
class CFont;

class CStatus : public IUpdateHandler
{
public:
	CStatus(CDoomRPG *doomrpg = nullptr) { Initialize(doomrpg); }
	~CStatus();

	bool Initialize(CDoomRPG *doomrpg);
	bool Update(float elapsedTime);
	void Render();
	void AddMessage(const std::string &message);

	void Show() { m_hidden = false; ToggleHUDVisibility(); }
	void Hide() { m_hidden = true; ToggleHUDVisibility(); }

	bool IsShown() const { return !m_hidden; }
	bool IsHidden() const { return m_hidden; }

	unsigned int GetHUD() const { return m_currentHud; }
	void SetHUD(unsigned int hud) { m_currentHud = hud; ChangeHUD(); }
	const std::string &GetMessage() const { return m_message; }
	void SetMessage(const std::string &message) { m_message = message; }

	bool IsInitialized() const { return m_initialized; }

private:
	void ChangeHUD();
	void ToggleHUDVisibility();
	void ShowHUD();
	void HideHUD();

	bool m_initialized{false};
	CDoomRPG *m_doomrpg{nullptr};
	CUIElement m_statusBar;
	CUIElement m_hudBars[2];
	CUIElement *m_huds[2]{nullptr};
	CUIElementStateful *m_faces[2]{nullptr};
	CUIElement *m_healthIcons[2]{nullptr};
	CUIElement *m_armorIcons[2]{nullptr};
	CUIElementStateful *m_weaponIcons[2]{nullptr};
	CUIElement *m_directionIcons[2]{nullptr};
	std::shared_ptr<CFont> m_largeFont;
	std::queue<std::string> m_messages;
	std::string m_message;
	unsigned int m_displayTics{0}, m_displayTicCount{0};
	bool m_hidden{false};
	unsigned int m_currentFace{0}, m_lastFace{0};
	unsigned int m_currentWeaponIcon{0}, m_lastWeaponIcon{0};
	unsigned int m_currentHud{0};
};

#endif