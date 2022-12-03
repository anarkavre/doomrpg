#include "CAnimationSystem.h"
#include "Game/CGame.h"
#include "Resource/CAnimation.h"
#include "Resource/CSound.h"

CAnimationSystem::~CAnimationSystem()
{
	if (m_initialized)
		m_game->GetGameLoop().UnregisterUpdateHandler(*this);
}

bool CAnimationSystem::Initialize(CGame *game)
{
	if (ISystem::Initialize(game))
	{
		m_game->GetGameLoop().RegisterUpdateHandler(*this);

		m_initialized = true;
	}

	return m_initialized;
}

bool CAnimationSystem::Update(float elapsedTime)
{
	for (auto it = m_textureAnimationHandles.begin(); it != m_textureAnimationHandles.end(); ++it)
	{
		unsigned int handle = *it;
		TextureAnimationData &textureAnimation = m_textureAnimationPool.Get(handle);

		if (textureAnimation.stateStarted)
		{
			const AnimationState *currentState = &textureAnimation.animation->GetState(textureAnimation.currentState);

			if (!currentState->sound.empty())
				textureAnimation.sounds[currentState->sound]->Play();

			textureAnimation.stateStarted = false;
		}

		if (textureAnimation.frameStarted)
		{
			const AnimationState *currentState = &textureAnimation.animation->GetState(textureAnimation.currentState);
			const AnimationFrame *currentFrame = &currentState->frames[textureAnimation.currentFrame];

			if (!currentFrame->sound.empty())
				textureAnimation.sounds[currentFrame->sound]->Play();

			textureAnimation.frameStarted = false;
		}

		textureAnimation.tics--;

		if (textureAnimation.tics == 0)
		{
			const AnimationState *currentState = &textureAnimation.animation->GetState(textureAnimation.currentState);

			textureAnimation.currentFrame = (textureAnimation.currentFrame + 1) % currentState->frames.size();

			if (textureAnimation.currentFrame == 0)
			{
				textureAnimation.loops--;

				if (textureAnimation.loops == 0)
				{
					textureAnimation.currentState = currentState->next;
					currentState = &textureAnimation.animation->GetState(textureAnimation.currentState);
					textureAnimation.loops = currentState->loops;
				}

				textureAnimation.stateStarted = true;
			}

			const AnimationFrame *currentFrame = &currentState->frames[textureAnimation.currentFrame];
			textureAnimation.tics = static_cast<unsigned int>(currentFrame->duration * m_game->GetGameLoop().GetUpdateRate());
			textureAnimation.frameStarted = true;
		}
	}

	return false;
}

bool CAnimationSystem::CreateTextureAnimation(const std::string &name, unsigned int &handle)
{
	TextureAnimationData textureAnimation;
	textureAnimation.animation = m_game->GetResourceManager().AcquireResource<CAnimation>(name + ".json");
	textureAnimation.currentState = textureAnimation.animation->GetInitialState().name;
	textureAnimation.currentFrame = 0;
	textureAnimation.loops = textureAnimation.animation->GetInitialState().loops;
	textureAnimation.tics = static_cast<unsigned int>(textureAnimation.animation->GetFrame(textureAnimation.currentState, textureAnimation.currentFrame).duration * m_game->GetGameLoop().GetUpdateRate());
	textureAnimation.stateStarted = true;
	textureAnimation.frameStarted = true;

	std::vector<std::string> stateNames;

	textureAnimation.animation->GetStateNames(stateNames);

	for (unsigned int i = 0; i < stateNames.size(); i++)
	{
		const AnimationState &state = textureAnimation.animation->GetState(stateNames[i]);

		if (!state.sound.empty() && textureAnimation.sounds.count(state.sound) == 0)
			textureAnimation.sounds.insert(std::make_pair(state.sound, m_game->GetResourceManager().AcquireResource<CSound>(state.sound)));

		for (unsigned int j = 0; j < state.frames.size(); j++)
		{
			const AnimationFrame &frame = state.frames[j];

			if (!frame.sound.empty() && textureAnimation.sounds.count(frame.sound) == 0)
				textureAnimation.sounds.insert(std::make_pair(frame.sound, m_game->GetResourceManager().AcquireResource<CSound>(frame.sound)));
		}
	}

	handle = m_textureAnimationPool.Add(textureAnimation);
	m_textureAnimationHandles.push_back(handle);

	return true;
}

void CAnimationSystem::DestroyTextureAnimation(unsigned int handle)
{
	m_textureAnimationHandles.remove(handle);
	m_textureAnimationPool.Remove(handle);
}

TextureAnimationData &CAnimationSystem::GetTextureAnimation(unsigned int handle)
{
	return m_textureAnimationPool.Get(handle);
}