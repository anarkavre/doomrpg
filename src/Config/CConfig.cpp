#include <fstream>
#include <stdexcept>

#include "CConfig.h"
#include "Game/CGame.h"

bool CConfig::Initialize(CGame *game, const std::string &name)
{
	if (!m_initialized)
	{
		m_name = name;

		if (m_name.empty())
			throw std::invalid_argument("Config name is empty");

		CConsole &console = game->GetConsole();

		console.Write("Opening config \"" + m_name + "\"...");

		std::ifstream m_ifstream;

		m_ifstream.open(m_name.c_str(), std::ifstream::in);

		if (!m_ifstream.is_open())
		{
			m_ifstream.open(m_name.c_str(), std::ifstream::in | std::ifstream::out | std::ifstream::trunc);

			if (!m_ifstream.is_open())
			{
				console.Write("Failed\n");

				throw std::runtime_error("Could not open \"" + m_name + "\"");
			}
		}

		console.Write("OK\n");

		while (!m_ifstream.eof())
		{
			std::string variable;
			m_ifstream >> variable;

			if (!variable.empty())
			{
				std::string name = variable.substr(0, variable.find_first_of('='));
				std::string value = variable.substr(variable.find_first_of('=') + 1);
				m_variables.insert(std::make_pair(name, value));
			}
		}

		if (m_ifstream.is_open())
			m_ifstream.close();

		m_initialized = true;
	}

	return m_initialized;
}

CConfig::~CConfig()
{
	Save();
}

void CConfig::Save()
{
	if (!m_initialized)
		return;

	std::ofstream m_ofstream;

	m_ofstream.open(m_name.c_str(), std::ofstream::out);

	for (auto it = m_variables.cbegin(); it != m_variables.cend(); ++it)
		m_ofstream << (*it).first << "=" << (*it).second << "\n";

	if (m_ofstream.is_open())
		m_ofstream.close();
}