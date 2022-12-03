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
	CDialog(CDoomRPG *doomrpg = nullptr) : m_initialized(false), m_hidden(true) { Initialize(doomrpg); }
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

	bool m_initialized;
	CDoomRPG *m_doomrpg;
	CUIElement m_dialog;
	CInputAction *m_actions[3];
	std::string m_message;
	bool m_hidden;
	bool m_locked;
	bool m_canceled;
	std::vector<std::string> m_lines;
	unsigned int m_start, m_end;
	unsigned int m_index;
	unsigned int m_printTics, m_printTicCount;
	bool m_printing;
};

#endif