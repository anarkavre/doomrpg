#include <stdexcept>

#include "CResource.h"
#include "Game/CGame.h"

CResource::CResource(CGame *game, unsigned int handle, const std::string &name, const std::string &path, ECResourceType type, bool openFile) : m_game(game), m_handle(handle), m_name(name), m_path(game->GetResourceManager().GetBasePath() + path), m_type(type)
{
	if (m_name.empty())
		throw std::invalid_argument("Resource name is empty");

	if (!openFile)
		return;

	CConsole &console = m_game->GetConsole();

	std::string filename = m_path + m_name;

	console.Write("Loading resource \"" + filename + "\"...");

	m_ifstream.open(filename.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!m_ifstream.is_open())
	{
		console.Write("Failed\n");
		
		throw std::runtime_error("Could not open \"" + filename + "\"");
	}

	m_ifstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	m_ifstream.seekg(0, std::ifstream::end);
	m_size = static_cast<unsigned long>(m_ifstream.tellg());
	m_ifstream.seekg(0, std::ifstream::beg);
}

CResource::~CResource()
{
	if (m_ifstream.is_open())
		m_ifstream.close();
	
	if (m_game != nullptr && m_handle != UINT_MAX)
		m_game->GetResourceManager().DestroyResource(m_handle);
}

void CResource::CloseFile()
{
	if (m_ifstream.is_open())
	{
		m_game->GetConsole().Write("OK\n");

		m_ifstream.close();
	}
}

unsigned char *CResource::GetData(unsigned char *data, unsigned long size, unsigned long *count)
{
	if (data == nullptr || size == 0 || !m_ifstream.is_open())
		return nullptr;

	try
	{
		m_ifstream.read(reinterpret_cast<char *>(data), size);

		if (count != nullptr)
			*count = static_cast<unsigned long>(m_ifstream.gcount());

		if (static_cast<unsigned long>(m_ifstream.tellg()) >= m_size)
		{
			m_game->GetConsole().Write("OK\n");

			m_ifstream.close();
		}
	}
	catch (const std::ifstream::failure &e)
	{
		m_game->GetConsole().Write("Failed\n");

		return nullptr;
	}

	return data;
}