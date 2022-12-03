#ifndef __CANIMATIONSYSTEM_H__
#define __CANIMATIONSYSTEM_H__

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include "Interface/ISystem.h"
#include "Interface/IUpdateHandler.h"
#include "Pool/CPool.h"

class CAnimation;
class CSound;

struct TextureAnimationData
{
	std::shared_ptr<CAnimation> animation;
	std::unordered_map<std::string, std::shared_ptr<CSound>> sounds;
	std::string currentState;
	unsigned int currentFrame;
	unsigned int loops;
	unsigned int tics;
	bool stateStarted;
	bool frameStarted;
};

class CAnimationSystem : public ISystem, public IUpdateHandler
{
public:
	CAnimationSystem(CGame *game = nullptr) { Initialize(game); }
	~CAnimationSystem();

	bool Initialize(CGame *game);
	bool Update(float elapsedTime);

	bool CreateTextureAnimation(const std::string &name, unsigned int &handle);
	void DestroyTextureAnimation(unsigned int handle);
	TextureAnimationData &GetTextureAnimation(unsigned int handle);

private:
	CPool<TextureAnimationData> m_textureAnimationPool;
	std::list<unsigned int> m_textureAnimationHandles;
};

#endif