#include <algorithm>
#include <stdexcept>

#include "CConsoleVariable.h"

void CConsoleVariable::ProcessValue()
{
	if (m_type == ECConsoleVariableType::Bool)
	{
		std::transform(m_value.begin(), m_value.end(), m_value.begin(), ::tolower);

		if (m_value == "true" || m_value == "false")
			return;

		try
		{
			m_value = (std::stoi(m_value) == 0 ? "false" : "true");
		}
		catch (const std::invalid_argument &e)
		{
			m_value = "false";
		}
	}
	else if (m_type == ECConsoleVariableType::Float)
	{
		try
		{
			m_value = std::to_string(std::stof(m_value));
		}
		catch (const std::invalid_argument &e)
		{
			m_value = "0.000000";
		}
	}
	else if (m_type == ECConsoleVariableType::Int)
	{
		std::transform(m_value.begin(), m_value.end(), m_value.begin(), ::tolower);

		if (m_value == "true" || m_value == "false")
		{
			m_value = (m_value == "true" ? "1" : "0");
			return;
		}

		try
		{
			m_value = std::to_string(std::stoi(m_value));
		}
		catch (const std::invalid_argument &e)
		{
			m_value = "0";
		}
	}
}