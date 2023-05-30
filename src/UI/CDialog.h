#ifndef __CDIALOG_H__
#define __CDIALOG_H__

#include <memory>
#include <string>
#include <vector>

#include "Interface/IUpdateHandler.h"
#include "UI/UIElement.h"

class CDoomRPG;
class CInputAction;

class CDialog : public IUpdateHandler
{
public:
	CDialog(CDoomRPG *doomrpg = nullptr) { Initialize(doomrpg); }
	~CDialog();

	bool Initialize(CDoomRPG *doomrpg);
	bool Update(float elapsedTime);
	void Render();

	void Show();
	void Hide();
	void Lock() { m_locked = true; }
	void Unlock() { m_locked = false; }

	bool IsShown() const { return !m_hidden; }
	bool IsHidden() const { return m_hidden; }
	bool IsLocked() const { return m_locked; }
	bool IsCanceled() const { return m_canceled; }

	const std::string &GetMessage() const { return m_message; }
	void SetMessage(const std::string &message) { m_message = message; ParseLines(); }

	bool IsInitialized() const { return m_initialized; }

private:
	void ParseLines();

	bool m_initialized{false};
	CDoomRPG *m_doomrpg{nullptr};
	CUIElement m_dialog;
	CInputAction *m_actions[3]{nullptr};
	std::string m_message;
	bool m_hidden{true};
	bool m_locked{false};
	bool m_canceled{false};
	std::vector<std::string> m_lines;
	unsigned int m_start{0}, m_end{0};
	unsigned int m_index{0};
	unsigned int m_printTics{0}, m_printTicCount{0};
	bool m_printing{false};
};

#endif