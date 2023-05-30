#ifndef __CTEXTUREANIMATION_H__
#define __CTEXTUREANIMATION_H__

#include <string>

class CAnimation;
class CGame;

class CTextureAnimation
{
public:
	CTextureAnimation() {}
	CTextureAnimation(const CTextureAnimation &textureAnimation) = default;
	CTextureAnimation(CTextureAnimation &&textureAnimation) noexcept : m_initialized(std::exchange(textureAnimation.m_initialized, false)), m_game(textureAnimation.m_game), m_handle(textureAnimation.m_handle) {}
	CTextureAnimation(CGame *game, const std::string &name) { Initialize(game, name); }
	~CTextureAnimation();

	bool Initialize(CGame *game, const std::string &name);

	const CAnimation *GetAnimation() const;
	std::string GetState() const;
	void SetState(const std::string &name);
	unsigned int GetFrame() const;
	int GetHorizontalOffset() const;
	int GetVerticalOffset() const;

	bool IsInitialized() const { return m_initialized; }

private:
	bool m_initialized{false};
	CGame *m_game{nullptr};
	unsigned int m_handle{0};
};

#endif