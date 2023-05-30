#ifndef __CCONSOLE_H__
#define __CCONSOLE_H__

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

#include "CConsoleCommand.h"
#include "CConsoleDefaults.h"
#include "CConsoleVariable.h"
#include "Interface/IEventHandler.h"
#include "Interface/IUpdateHandler.h"

class CConfig;
class CGame;
class CFont;

enum class ECConsoleState
{
	Closed,
	Opening,
	Opened,
	Closing
};

class CConsole : public IEventHandler, public IUpdateHandler
{
public:
	CConsole(CGame *game = nullptr, unsigned int columns = defaultColumns, unsigned int rows = defaultRows, unsigned int maxLines = defaultMaxLines, unsigned int maxInputChars = defaultMaxInputChars, float speed = defaultSpeed, const std::string &font = "");
	~CConsole();

	bool Initialize(CGame *game, const std::string &font = "");
	bool ProcessEvent(const SDL_Event &event);
	bool Update(float elapsedTime);
	void Resize(unsigned int columns, unsigned int rows);
	void Write(const std::string &text);
	void UpdateInput(char c);
	CConsoleCommand *RegisterCommand(const std::string &name, const std::function<void (CConsole &, const std::vector<std::string> &)> &func);
	void UnregisterCommand(const std::string &name);
	CConsoleVariable *RegisterVariable(const std::string &name, bool value, const std::function<void (bool &)> &func);
	CConsoleVariable *RegisterVariable(const std::string &name, float value, const std::function<void (float &)> &func);
	CConsoleVariable *RegisterVariable(const std::string &name, int value, const std::function<void (int &)> &func);
	CConsoleVariable *RegisterVariable(const std::string &name, const std::string &value, const std::function<void (std::string &)> &func);
	void UnregisterVariable(const std::string &name);
	void LoadVariables(CConfig &config);
	void SaveVariables(CConfig &config);

	void ScrollUp();
	void ScrollDown();

	void Open() { m_state = ECConsoleState::Opening; }
	void Close() { m_state = ECConsoleState::Closing; }

	bool IsOpened() const { return (m_state == ECConsoleState::Opened); }
	bool IsClosed() const { return (m_state == ECConsoleState::Closed); }
	bool IsOpening() const { return (m_state == ECConsoleState::Opening); }
	bool IsClosing() const { return (m_state == ECConsoleState::Closing); }
	
	const glm::uvec4 &GetFGColor() const { return m_fgColor; }
	void SetFGColor(const glm::uvec4 &fgColor) { m_fgColor = fgColor; }
	const glm::uvec4 &GetBGColor() const { return m_fgColor; }
	void SetBGColor(const glm::uvec4 &bgColor) { m_bgColor = bgColor; }
	ECConsoleState GetState() const { return m_state; }
	void SetState(ECConsoleState state) { m_state = state; }
	const CConsoleCommand &GetCommand(const std::string &name) const { return m_commands.at(name); }
	const CConsoleVariable &GetVariable(const std::string &name) const { return m_variables.at(name); }

	bool IsInitialized() const { return m_initialized; }

private:
	void AddLine(const std::string &line);
	void ParseCommandAndArguments(const std::string &input, std::string &command, std::vector<std::string> &arguments);
	void ExecuteCommand(const std::string &command, const std::vector<std::string> &arguments);

	bool m_initialized{false};
	CGame *m_game{nullptr};
	unsigned int m_columns{defaultColumns}, m_rows{defaultRows};
	unsigned int m_maxLines{defaultMaxLines}, m_maxInputChars{defaultMaxInputChars};
	float m_speed{defaultSpeed};
	std::list<std::string> m_lines;
	std::string m_input, m_temp;
	glm::uvec4 m_fgColor{255, 255, 255, 255}, m_bgColor{44, 117, 151, 64};
	std::shared_ptr<CFont> m_font;
	bool m_updateConsole{false};
	bool m_updateFirstRow{false}, m_updateSecondRow{false}, m_updateLastRow{false};
	unsigned int m_rowUpdateCount{0}, m_charUpdateCount{0};
	bool m_carriageReturn{false}, m_backspace{false};
	std::list<std::string>::const_iterator m_bottomLine, m_topLine, m_lastLine;
	ECConsoleState m_state{ECConsoleState::Closed};
	float m_offsetY{0.0f};
	float m_bottom{0.0f}, m_top{0.0f};
	unsigned int m_blinkTics{0}, m_blinkTicCount{0};
	bool m_showCursor{false};
	std::unordered_map<std::string, CConsoleCommand> m_commands;
	std::unordered_map<std::string, CConsoleVariable> m_variables;
};

#endif