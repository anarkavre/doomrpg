#ifndef __CFONT_H__
#define __CFONT_H__

#include <memory>

#include "glm/glm.hpp"

#include "CResource.h"

class CTexture;

class CFont : public CResource
{
public:
	CFont(CGame *game, unsigned int handle, const std::string &name);
	~CFont();

	unsigned int GetFontWidth() const { return m_fontWidth; }
	unsigned int GetFontHeight() const { return m_fontHeight; }
	unsigned int GetCellWidth() const { return m_cellWidth; }
	unsigned int GetCellHeight() const { return m_cellHeight; }
	unsigned int GetRowPitch() const { return m_rowPitch; }
	float GetColumnFactor() const { return m_columnFactor; }
	float GetRowFactor() const { return m_rowFactor; }
	char GetBase() const { return m_base; }
	unsigned int GetWidth(const std::string &text) const;
	unsigned int GetWidth(char c) const;
	CTexture *GetTexture() const { return m_texture.get(); }
	const glm::uvec4 &GetFGColor() const { return m_fgColor; }
	void SetFGColor(const glm::uvec4 &fgColor) { m_fgColor = fgColor; }
	const glm::uvec4 &GetBGColor() const { return m_bgColor; }
	void SetBGColor(const glm::uvec4 &bgColor) { m_bgColor = bgColor; }

	static const std::string &GetResourcePath() { return m_resourcePath; }

private:
	unsigned int m_fontWidth{0}, m_fontHeight{0};
	unsigned int m_cellWidth{0}, m_cellHeight{0};
	unsigned int m_rowPitch{0};
	float m_columnFactor{0.0f};
	float m_rowFactor{0.0f};
	unsigned char m_bpp{0};
	char m_base{0};
	unsigned char m_widthData[256]{0};
	std::shared_ptr<CTexture> m_texture;
	glm::uvec4 m_fgColor{255, 255, 255, 255}, m_bgColor{0, 0, 0, 0};

	static const std::string m_resourcePath;
};

#endif