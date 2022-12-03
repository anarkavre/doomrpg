#ifndef __CTEXTURE_H__
#define __CTEXTURE_H__

#include "CResource.h"
#include "Interface/IRenderer.h"

class IRenderer;

class CTexture : public CResource
{
public:
	CTexture(CGame *game, unsigned int handle, const std::string &name);
	CTexture(CGame *game, const void *data, unsigned int width, unsigned int height, unsigned int bpp);
	CTexture(CGame *game, const void *data, unsigned int width, unsigned int height, unsigned int tileColumnCount, unsigned int tileRowCount);
	~CTexture();

	void Bind();
	void GenerateMipmaps();

	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }
	ECTextureFilter GetFilter() const { return m_filter; }
	void SetFilter(ECTextureFilter filter) { m_filter = filter; SetFilterMode(); }
	ECTextureWrap GetWrap(ECTextureCoord coord) const { return m_wrap[static_cast<unsigned int>(coord)]; }
	void SetWrap(ECTextureCoord coord, ECTextureWrap wrap) { m_wrap[static_cast<unsigned int>(coord)] = wrap; SetWrapMode(); }
	void SetWrap(ECTextureWrap wrap) { m_wrap[static_cast<unsigned int>(ECTextureCoord::U)] = m_wrap[static_cast<unsigned int>(ECTextureCoord::V)] = wrap; SetWrapMode(); }

	static const std::string &GetResourcePath() { return m_resourcePath; }

private:
	void SetFilterMode();
	void SetWrapMode();

	IRenderer *m_renderer;
	unsigned int m_handle;
	unsigned int m_width, m_height;
	ECTextureFilter m_filter;
	ECTextureWrap m_wrap[2];
	bool m_hasMipmaps;

	static const std::string m_resourcePath;
};

#endif