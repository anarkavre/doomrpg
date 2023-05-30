#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>

#include "CConsole.h"
#include "Game/CGame.h"
#include "Interface/IRenderer.h"
#include "Resource/CFont.h"
#include "Window/CWindow.h"

CConsole::CConsole(CGame *game, unsigned int columns, unsigned int rows, unsigned int maxLines, unsigned int maxInputChars, float speed, const std::string &font) : m_columns(columns), m_rows(rows), m_maxLines(maxLines), m_maxInputChars(maxInputChars), m_speed(speed)
{
	if (m_columns < defaultMinColumns)
		m_columns = defaultMinColumns;
	if (m_columns > defaultMaxColumns)
		m_columns = defaultMaxColumns;
	if (m_rows < defaultMinRows)
		m_rows = defaultMinRows;
	if (m_rows > defaultMaxRows)
		m_rows = defaultMaxRows;
	if (m_maxLines < m_rows - 1)
		m_maxLines = (m_rows - 1) * 2;
	if (m_maxInputChars < defaultMaxInputChars)
		m_maxInputChars = defaultMaxInputChars;
	if (m_speed == 0.0f)
		m_speed = defaultSpeed;
	if (m_speed < 0.0f)
		m_speed = fabsf(m_speed);
	
	RegisterCommand("get",
		[this](CConsole &console, const std::vector<std::string> &arguments)
		{
			if (arguments.size() == 1)
			{
				std::string name = arguments[0];
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);

				if (m_variables.count(name) == 1)
				{
					const CConsoleVariable &variable = m_variables[name];
					console.Write(variable.GetName() + "=" + variable.GetValue() + "\n");
				}
				else
					console.Write("Unknown Variable\n");
			}
			else
				console.Write("Get Variable Usage: get <name>\n");
		}
	);
	RegisterCommand("set",
		[this](CConsole &console, const std::vector<std::string> &arguments)
		{
			if (arguments.size() == 2)
			{
				std::string name = arguments[0];
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);

				if (m_variables.count(name) == 1)
				{
					CConsoleVariable &variable = m_variables[name];
					variable.SetValue(arguments[1]);
					console.Write(variable.GetName() + "=" + variable.GetValue() + "\n");
				}
				else
					console.Write("Unknown Variable\n");
			}
			else
				console.Write("Set Variable Usage: set <name> <value>\n");
		}
	);

	Initialize(game, font);
}

CConsole::~CConsole()
{
	if (m_game != nullptr)
	{
		m_game->GetGameLoop().UnregisterEventHandler(*this, m_game->GetWindow().GetID());
		m_game->GetGameLoop().UnregisterUpdateHandler(*this);
	}
}

bool CConsole::Initialize(CGame *game, const std::string &font)
{
	if (!m_initialized && game != nullptr)
	{
		m_game = game;

		m_font = m_game->GetResourceManager().AcquireResource<CFont>(font.empty() ? defaultConsoleFont : font);

		m_updateConsole = false;
		m_updateFirstRow = false;
		m_updateSecondRow = false;
		m_updateLastRow = false;
		m_rowUpdateCount = 0;
		m_charUpdateCount = 0;
		m_carriageReturn = false;
		m_backspace = false;
		m_offsetY = 0.0f;
		m_top = 0.0f;
		m_bottom = m_top + m_rows * m_font->GetCellHeight();
		m_blinkTics = static_cast<unsigned int>(0.5f * m_game->GetGameLoop().GetUpdateRate());

		ConsoleData &consoleData = m_game->GetRenderSystem().GetConsoleData();
		consoleData.initialize = true;
		consoleData.font = m_font;
		consoleData.fgColor = &m_fgColor;
		consoleData.bgColor = &m_bgColor;
		consoleData.columns = m_columns;
		consoleData.rows = m_rows;
		consoleData.lines = &m_lines;
		consoleData.input = &m_input;
		consoleData.bottomLine = &m_bottomLine;
		consoleData.topLine = &m_topLine;
		consoleData.bottom = m_bottom;
		consoleData.top = m_top;
		consoleData.updateFirstRow = false;
		consoleData.updateSecondRow = false;
		consoleData.updateLastRow = false;
		consoleData.rowUpdateCount = 0;
		consoleData.charUpdateCount = 0;
		consoleData.carriageReturn = false;
		consoleData.backspace = false;
		consoleData.showCursor = false;
		m_game->GetRenderSystem().UpdateConsole(true);
		
		m_game->GetGameLoop().RegisterEventHandler(*this, m_game->GetWindow().GetID());
		m_game->GetGameLoop().RegisterUpdateHandler(*this);

		m_initialized = true;
	}

	return m_initialized;
}

bool CConsole::ProcessEvent(const SDL_Event &event)
{
	if (!m_initialized)
		return false;

	switch (event.type)
	{
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_BACKQUOTE:
			if (IsClosed())
				Open();
			else
				Close();
			break;
		case SDLK_PAGEUP:
			ScrollUp();
			break;
		case SDLK_PAGEDOWN:
			ScrollDown();
			break;
		case SDLK_RETURN:
			UpdateInput('\r');
			break;
		case SDLK_BACKSPACE:
			UpdateInput('\b');
			break;
		default:
			break;
		}
		break;
	case SDL_TEXTINPUT:
		UpdateInput(event.text.text[0]);
		break;
	default:
		break;
	}

	return false;
}

bool CConsole::Update(float elapsedTime)
{
	if (!m_initialized)
		return false;

	if (m_state == ECConsoleState::Opened)
	{
		m_blinkTicCount--;

		if (m_blinkTicCount == 0)
		{
			m_showCursor = !m_showCursor;
			m_blinkTicCount = m_blinkTics;

			ConsoleData &consoleData = m_game->GetRenderSystem().GetConsoleData();
			consoleData.showCursor = m_showCursor;
		}
	}

	if ((m_state == ECConsoleState::Opened || m_state == ECConsoleState::Opening) && m_updateConsole)
	{
		ConsoleData &consoleData = m_game->GetRenderSystem().GetConsoleData();
		consoleData.updateFirstRow = m_updateFirstRow;
		consoleData.updateSecondRow = m_updateSecondRow;
		consoleData.updateLastRow = m_updateLastRow;
		consoleData.rowUpdateCount = m_rowUpdateCount;
		consoleData.charUpdateCount = m_charUpdateCount;
		consoleData.carriageReturn = m_carriageReturn;
		consoleData.backspace = m_backspace;
		m_game->GetRenderSystem().UpdateConsole(true);

		m_updateConsole = false;
		m_updateFirstRow = false;
		m_updateSecondRow = false;
		m_updateLastRow = false;
		m_rowUpdateCount = 0;
		m_charUpdateCount = 0;
		m_carriageReturn = false;
		m_backspace = false;
	}
	else if (m_state == ECConsoleState::Opening)
	{
		float distance = m_speed * elapsedTime;

		m_offsetY += distance;

		if (m_offsetY > m_bottom)
			m_offsetY = m_bottom;
		if (m_offsetY == m_bottom)
		{
			SDL_StartTextInput();
			m_blinkTicCount = m_blinkTics;
			m_state = ECConsoleState::Opened;
		}

		m_game->GetRenderSystem().MoveConsoleDown(distance);
	}
	else if (m_state == ECConsoleState::Closing)
	{
		float distance = m_speed * elapsedTime;

		if (m_offsetY == m_bottom)
		{
			SDL_StopTextInput();
			m_showCursor = false;

			ConsoleData &consoleData = m_game->GetRenderSystem().GetConsoleData();
			consoleData.showCursor = false;
		}

		m_offsetY -= distance;

		if (m_offsetY < m_top)
			m_offsetY = m_top;
		if (m_offsetY == m_top)
			m_state = ECConsoleState::Closed;

		m_game->GetRenderSystem().MoveConsoleUp(distance);
	}

	return false;
}

void CConsole::Resize(unsigned int columns, unsigned int rows)
{
	m_columns = columns;
	m_rows = rows;

	if (m_columns < defaultMinColumns)
		m_columns = defaultMinColumns;
	if (m_columns > defaultMaxColumns)
		m_columns = defaultMaxColumns;
	if (m_rows < defaultMinRows)
		m_rows = defaultMinRows;
	if (m_rows > defaultMaxRows)
		m_rows = defaultMaxRows;
	if (m_maxLines < m_rows - 1)
		m_maxLines = (m_rows - 1) * 2;

	if (!m_lines.empty())
	{
		std::list<std::string> lines = m_lines;

		if (!m_temp.empty())
		{
			lines.push_back(m_temp);
			m_temp.clear();
		}

		m_lines.clear();

		std::list<std::string>::const_iterator it = lines.cend();

		while (it != lines.cbegin())
		{
			std::string line;

			line += *(--it);

			if (line.back() == '\n')
			{
				Write(line);

				line.clear();
			}
		}
	}

	if (m_initialized)
	{
		m_updateConsole = false;
		m_updateFirstRow = false;
		m_updateSecondRow = false;
		m_updateLastRow = false;
		m_rowUpdateCount = 0;
		m_charUpdateCount = 0;
		m_carriageReturn = false;
		m_backspace = false;
		m_offsetY = 0.0f;
		m_top = 0.0f;
		m_bottom = m_top + m_rows * m_font->GetCellHeight();

		ConsoleData &consoleData = m_game->GetRenderSystem().GetConsoleData();
		consoleData.initialize = true;
		consoleData.font = m_font;
		consoleData.fgColor = &m_fgColor;
		consoleData.bgColor = &m_bgColor;
		consoleData.columns = m_columns;
		consoleData.rows = m_rows;
		consoleData.lines = &m_lines;
		consoleData.input = &m_input;
		consoleData.bottomLine = &m_bottomLine;
		consoleData.topLine = &m_topLine;
		consoleData.bottom = m_bottom;
		consoleData.top = m_top;
		consoleData.updateFirstRow = false;
		consoleData.updateSecondRow = false;
		consoleData.updateLastRow = false;
		consoleData.rowUpdateCount = 0;
		consoleData.charUpdateCount = 0;
		consoleData.carriageReturn = false;
		consoleData.backspace = false;
		consoleData.showCursor = false;
		m_game->GetRenderSystem().UpdateConsole(true);
	}
}

void CConsole::Write(const std::string &text)
{
	static std::string temp;
	std::string line = temp + text;
	temp.clear();
	size_t position = line.find('\n');

	if (position != std::string::npos && position <= m_columns)
	{
		AddLine(line.substr(0, position + 1));
		
		if (position != line.length() - 1)
			Write(line.substr(position + 1));
	}
	else if (line.length() <= m_columns)
		temp = line;
	else
	{
		AddLine(line.substr(0, m_columns));
		Write(line.substr(m_columns));
	}
}

void CConsole::UpdateInput(char c)
{
	if (!m_initialized || m_state != ECConsoleState::Opened)
		return;

	m_updateConsole = true;
	m_updateFirstRow = true;
	
	if (c != '\r' && c != '\b' && m_input.length() < m_maxInputChars)
	{
		m_input.push_back(c);
		m_charUpdateCount++;

		if (m_input.length() < m_columns - 1)
			m_game->GetRenderSystem().MoveCursorRight();
	}
	else if (c == '\r')
	{
		std::string command;
		std::vector<std::string> arguments;
		ParseCommandAndArguments(m_input, command, arguments);
		ExecuteCommand(command, arguments);
		m_input.clear();
		m_charUpdateCount = 0;
		m_carriageReturn = true;
		m_game->GetRenderSystem().ResetCursor();
	}
	else if (c == '\b' && m_input.length() > 0)
	{
		if (m_input.length() < m_columns - 1)
			m_game->GetRenderSystem().MoveCursorLeft();

		m_input.pop_back();
		m_backspace = true;
	}
	else
	{
		m_updateConsole = false;
		m_updateFirstRow = false;
	}
}

CConsoleCommand *CConsole::RegisterCommand(const std::string &name, const std::function<void (CConsole &, const std::vector<std::string> &)> &func)
{
	m_commands.insert(std::make_pair(name, CConsoleCommand(name, func)));
	return &m_commands[name];
}

void CConsole::UnregisterCommand(const std::string &name)
{
	m_commands.erase(name);
}

CConsoleVariable *CConsole::RegisterVariable(const std::string &name, bool value, const std::function<void (bool &)> &func)
{
	m_variables.insert(std::make_pair(name, CConsoleVariable(name, value, func)));
	return &m_variables[name];
}

CConsoleVariable *CConsole::RegisterVariable(const std::string &name, float value, const std::function<void (float &)> &func)
{
	m_variables.insert(std::make_pair(name, CConsoleVariable(name, value, func)));
	return &m_variables[name];
}

CConsoleVariable *CConsole::RegisterVariable(const std::string &name, int value, const std::function<void (int &)> &func)
{
	m_variables.insert(std::make_pair(name, CConsoleVariable(name, value, func)));
	return &m_variables[name];
}

CConsoleVariable *CConsole::RegisterVariable(const std::string &name, const std::string &value, const std::function<void (std::string &)> &func)
{
	m_variables.insert(std::make_pair(name, CConsoleVariable(name, value, func)));
	return &m_variables[name];
}

void CConsole::UnregisterVariable(const std::string &name)
{
	m_variables.erase(name);
}

void CConsole::LoadVariables(CConfig &config)
{
	if (config.GetCount() != 0)
		for (auto it = m_variables.begin(); it != m_variables.end(); ++it)
			if (!config.Get((*it).second.GetName()).empty())
				(*it).second.SetValue(config.Get((*it).second.GetName()));
}

void CConsole::SaveVariables(CConfig &config)
{
	if (m_variables.size() != 0)
		for (auto it = m_variables.cbegin(); it != m_variables.cend(); ++it)
			config.Set((*it).second.GetName(), (*it).second.GetValue());
}

void CConsole::ScrollUp()
{
	if (m_state == ECConsoleState::Opened && m_lines.size() > m_rows - 1 && m_topLine != m_lastLine)
	{
		++m_topLine;
		++m_bottomLine;
		m_updateConsole = true;
		m_updateLastRow = true;
	}
}

void CConsole::ScrollDown()
{
	if (m_state == ECConsoleState::Opened && !m_lines.empty() && m_bottomLine != m_lines.cbegin())
	{
		--m_topLine;
		--m_bottomLine;
		m_updateConsole = true;
		m_updateSecondRow = true;
	}
}

void CConsole::AddLine(const std::string &line)
{
	if (m_lines.size() == m_maxLines)
	{
		--m_lastLine;
		m_temp += m_lines.back();
		m_lines.pop_back();

		if (m_temp.back() == '\n')
			m_temp.clear();
	}

	m_lines.push_front(line);

	if (m_lines.size() > 1)
		--m_bottomLine;
	else
	{
		m_bottomLine = m_lines.cbegin();
		m_topLine = m_lines.cbegin();
		m_lastLine = m_lines.cbegin();
	}

	if (m_lines.size() > m_rows - 1)
		--m_topLine;
	
	m_updateConsole = true;
	m_updateSecondRow = true;
	m_rowUpdateCount++;
}

void CConsole::ParseCommandAndArguments(const std::string &input, std::string &command, std::vector<std::string> &arguments)
{
	std::stringstream ss(input);
	std::string argument;
	
	while (getline(ss, argument, ' '))
	{
		if (!argument.empty())
		{
			if (command.empty())
				command = argument;
			else
				arguments.push_back(argument);
		}
	}

	std::transform(command.begin(), command.end(), command.begin(), ::tolower);
}

void CConsole::ExecuteCommand(const std::string &command, const std::vector<std::string> &arguments)
{
	if (m_commands.count(command) == 1)
	{
		CConsoleCommand &consoleCommand = m_commands[command];
		consoleCommand(*this, arguments);
	}
	else
		Write("Unknown Command\n");
}