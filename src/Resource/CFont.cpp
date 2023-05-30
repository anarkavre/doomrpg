#include <stdexcept>

#include "CFont.h"
#include "CTexture.h"
#include "Font/Font.h"
#include "Game/CGame.h"
#include "Utility/JSON/JSON.h"

#define WIDTH_DATA_OFFSET 20
#define MAP_DATA_OFFSET 276

static bool ends_with(const std::string &value, const std::string &ending)
{
	if (ending.length() > value.length())
		return false;

	return equal(ending.rbegin(), ending.rend(), value.rbegin(), [](const char a, const char b) { return (tolower(a) == tolower(b)); });
}

CFont::CFont(CGame *game, unsigned int handle, const std::string &name) : CResource(game, handle, name, GetResourcePath(), ECResourceType::Font)
{
	CConsole &console = m_game->GetConsole();

	if (ends_with(m_name, ".bff"))
	{
		unsigned char *data = new unsigned char[GetSize()];
		GetData(data, GetSize());

		console.Write("Checking file signature...");

		if (data[0] != 0xBF && data[1] != 0xF2)
		{
			delete[] data;

			console.Write("Failed\n");

			throw std::runtime_error("\"" + m_path + m_name + "\" is not a valid font file");
		}

		console.Write("OK\n");

		m_fontWidth = *(reinterpret_cast<unsigned int *>(&data[2]));
		m_fontHeight = *(reinterpret_cast<unsigned int *>(&data[6]));
		m_cellWidth = *(reinterpret_cast<unsigned int *>(&data[10]));
		m_cellHeight = *(reinterpret_cast<unsigned int *>(&data[14]));
		m_bpp = data[18];
		m_base = data[19];

		console.Write("Checking file size...");

		if (GetSize() != (MAP_DATA_OFFSET + m_fontWidth * m_fontHeight * m_bpp / 8))
		{
			delete[] data;

			console.Write("Failed\n");

			throw std::runtime_error("\"" + m_path + m_name + "\" is not a valid font file");
		}

		console.Write("OK\n");

		memcpy(m_widthData, &data[WIDTH_DATA_OFFSET], 256);

		m_texture = std::shared_ptr<CTexture>(std::make_shared<CTexture>(m_game, &data[MAP_DATA_OFFSET], m_fontWidth, m_fontHeight, m_bpp));

		delete[] data;
	}
	else
	{
		unsigned char *data = new unsigned char[GetSize()];
		std::string text(reinterpret_cast<char *>(GetData(data, GetSize())), GetSize());
		delete[] data;

		Font font;

		JSON::Parse<Font>(text, font);

		m_texture = m_game->GetResourceManager().AcquireResource<CTexture>(font.texture);

		m_fontWidth = m_texture->GetWidth();
		m_fontHeight = m_texture->GetHeight();
		m_cellWidth = font.cellWidth;
		m_cellHeight = font.cellHeight;
		m_bpp = 32;
		m_base = font.baseCharacter;

		memset(m_widthData, font.characterWidth, 256);
	}

	m_rowPitch = m_fontWidth / m_cellWidth;
	m_columnFactor = m_cellWidth / static_cast<float>(m_fontWidth);
	m_rowFactor = m_cellHeight / static_cast<float>(m_fontHeight);

	m_texture->SetFilter(ECTextureFilter::Point);
	m_texture->SetWrap(ECTextureWrap::Clamp);
}

CFont::~CFont() {}

unsigned int CFont::GetWidth(const std::string &text) const
{
	unsigned int width = 0;
	const char *c_str = text.c_str();
	char c;

	while ((c = *(c_str++)) != 0)
		width += m_widthData[static_cast<unsigned char>(c)];

	return width;
}

unsigned int CFont::GetWidth(char c) const
{
	return m_widthData[static_cast<unsigned char>(c)];
}

const std::string CFont::m_resourcePath = "fonts/";