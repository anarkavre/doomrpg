#include <algorithm>
#include <stdexcept>

#include "SDL_image.h"

#include "CTexture.h"
#include "Game/CGame.h"
#include "Interface/IRenderer.h"

static bool ends_with(const std::string &value, const std::string &ending)
{
	if (ending.length() > value.length())
		return false;

	return equal(ending.rbegin(), ending.rend(), value.rbegin(), [](const char a, const char b) { return (tolower(a) == tolower(b)); });
}

CTexture::CTexture(CGame *game, unsigned int handle, const std::string &name) : CResource(game, handle, name, GetResourcePath(), ECResourceType::Texture), m_renderer(game->GetRenderer()), m_hasMipmaps(false)
{
	unsigned char *data = new unsigned char[GetSize()];
	GetData(data, GetSize());

	SDL_Surface *surface;
	
	if (ends_with(m_name, ".png"))
		surface = IMG_LoadPNG_RW(SDL_RWFromConstMem(data, GetSize()));
	else if (ends_with(m_name, ".bmp"))
		surface = IMG_LoadBMP_RW(SDL_RWFromConstMem(data, GetSize()));
	else
	{
		delete[] data;

		throw std::runtime_error("\"" + m_path + m_name + "\" is not a valid or supported image file");
	}

	delete[] data;

	if (surface->format->format != SDL_PIXELFORMAT_RGBA32)
	{
		if (ends_with(m_name, ".bmp") && surface->format->palette != nullptr)
		{
			bool colorKeyFound = false;

			for (int i = 0; i < surface->format->palette->ncolors; i++)
			{
				SDL_Color &color = surface->format->palette->colors[i];

				if (color.r == 255 && color.g == 0 && color.b == 255)
				{
					colorKeyFound = true;

					break;
				}
			}

			if (colorKeyFound)
				SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
		}

		SDL_Surface *surfaceRGBA = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);

		SDL_FreeSurface(surface);

		surface = surfaceRGBA;
	}

	m_width = surface->w;
	m_height = surface->h;

	m_renderer->CreateTexture(surface->pixels, m_width, m_height, 32, m_handle);

	SDL_FreeSurface(surface);

	SetFilter(ECTextureFilter::Bilinear);
	SetWrap(ECTextureWrap::Repeat);

	m_game->GetRenderSystem().RegisterTexture(*this);
}

CTexture::CTexture(CGame *game, const void *data, unsigned int width, unsigned int height, unsigned int bpp) : CResource(game), m_renderer(game->GetRenderer()), m_hasMipmaps(false)
{
	m_width = width;
	m_height = height;

	m_renderer->CreateTexture(data, m_width, m_height, bpp, m_handle);

	SetFilter(ECTextureFilter::Bilinear);
	SetWrap(ECTextureWrap::Repeat);

	m_game->GetRenderSystem().RegisterTexture(*this);
}

CTexture::CTexture(CGame *game, const void *data, unsigned int width, unsigned int height, unsigned int tileColumnCount, unsigned int tileRowCount) : CResource(game), m_renderer(game->GetRenderer()), m_hasMipmaps(false)
{
	m_width = width;
	m_height = height;

	m_renderer->CreateTextureArray(data, m_width, m_height, 32, tileColumnCount, tileRowCount, m_handle);

	SetFilter(ECTextureFilter::Bilinear);
	SetWrap(ECTextureWrap::Repeat);

	m_game->GetRenderSystem().RegisterTexture(*this);
}

CTexture::~CTexture()
{
	m_game->GetRenderSystem().UnregisterTexture(*this);

	m_renderer->DestroyTexture(m_handle);
}

void CTexture::Bind()
{
	m_renderer->BindTexture(m_handle);
}

void CTexture::GenerateMipmaps()
{
	if (m_hasMipmaps)
		return;

	m_renderer->GenerateTextureMipmaps(m_handle);

	m_hasMipmaps = true;
}

void CTexture::SetFilterMode()
{
	if (m_filter == ECTextureFilter::Trilinear && !m_hasMipmaps)
		m_filter = ECTextureFilter::Bilinear;

	m_renderer->SetTextureFilterMode(m_handle, m_filter);
}

void CTexture::SetWrapMode()
{
	m_renderer->SetTextureWrapMode(m_handle, ECTextureCoord::U, m_wrap[static_cast<unsigned int>(ECTextureCoord::U)]);
	m_renderer->SetTextureWrapMode(m_handle, ECTextureCoord::V, m_wrap[static_cast<unsigned int>(ECTextureCoord::V)]);
}

const std::string CTexture::m_resourcePath = "textures/";