#include "CTextureAnimation.h"
#include "Game/CGame.h"
#include "Resource/CAnimation.h"
#include "System/CAnimationSystem.h"

CTextureAnimation::~CTextureAnimation()
{
	if (m_initialized)
		m_game->GetAnimationSystem().DestroyTextureAnimation(m_handle);
}

bool CTextureAnimation::Initialize(CGame *game, const std::string &name)
{
	if (!m_initialized && game != nullptr)
	{
		m_game = game;

		m_game->GetAnimationSystem().CreateTextureAnimation(name, m_handle);

		m_initialized = true;
	}

	return m_initialized;
}

const CAnimation *CTextureAnimation::GetAnimation() const
{
	if (!m_initialized)
		return nullptr;

	TextureAnimationData &textureAnimation = m_game->GetAnimationSystem().GetTextureAnimation(m_handle);
	return textureAnimation.animation.get();
}

std::string CTextureAnimation::GetState() const
{
	if (!m_initialized)
		return "";

	TextureAnimationData &textureAnimation = m_game->GetAnimationSystem().GetTextureAnimation(m_handle);
	return textureAnimation.currentState;
}

void CTextureAnimation::SetState(const std::string &name)
{
	if (!m_initialized)
		return;

	TextureAnimationData &textureAnimation = m_game->GetAnimationSystem().GetTextureAnimation(m_handle);
	textureAnimation.currentState = name;
	textureAnimation.currentFrame = 0;
	textureAnimation.loops = textureAnimation.animation->GetState(textureAnimation.currentState).loops;
	textureAnimation.tics = static_cast<unsigned int>(textureAnimation.animation->GetFrame(textureAnimation.currentState, textureAnimation.currentFrame).duration * m_game->GetGameLoop().GetUpdateRate());
	textureAnimation.stateStarted = true;
	textureAnimation.frameStarted = true;
}

unsigned int CTextureAnimation::GetFrame() const
{
	if (!m_initialized)
		return 0;

	TextureAnimationData &textureAnimation = m_game->GetAnimationSystem().GetTextureAnimation(m_handle);
	return textureAnimation.animation->GetFrame(textureAnimation.currentState, textureAnimation.currentFrame).frame;
}

int CTextureAnimation::GetHorizontalOffset() const
{
	if (!m_initialized)
		return 0;

	TextureAnimationData &textureAnimation = m_game->GetAnimationSystem().GetTextureAnimation(m_handle);
	return textureAnimation.animation->GetFrame(textureAnimation.currentState, textureAnimation.currentFrame).horizontalOffset;
}

int CTextureAnimation::GetVerticalOffset() const
{
	if (!m_initialized)
		return 0;

	TextureAnimationData &textureAnimation = m_game->GetAnimationSystem().GetTextureAnimation(m_handle);
	return textureAnimation.animation->GetFrame(textureAnimation.currentState, textureAnimation.currentFrame).verticalOffset;
}