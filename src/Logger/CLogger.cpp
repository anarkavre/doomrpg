#include <stdexcept>

#include "CLogger.h"
#include "Game/CGame.h"

bool CLogger::Initialize(CGame *game, const std::string &name)
{
	if (!m_initialized)
	{
		m_name = name;

		if (m_name.empty())
			throw std::invalid_argument("Log name is empty");

		CConsole &console = game->GetConsole();

		console.Write("Opening log \"" + m_name + "\"...");

		m_ofstream.open(m_name.c_str(), std::ofstream::out | std::ofstream::app);

		if (!m_ofstream.is_open())
		{
			console.Write("Failed\n");

			throw std::runtime_error("Could not open \"" + m_name + "\"");
		}

		console.Write("OK\n");

		m_initialized = true;
	}

	return m_initialized;
}

CLogger::~CLogger()
{
	if (m_ofstream.is_open())
		m_ofstream.close();
}

void CLogger::Write(const std::string &text)
{
	if (!m_initialized)
		return;

	m_ofstream << text;

	if (text[text.length() - 1] == '\n')
		m_ofstream.flush();
}