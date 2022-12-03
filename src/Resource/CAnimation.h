#ifndef __CANIMATION_H__
#define __CANIMATION_H__

#include <string>

#include "CResource.h"
#include "Animation/Animation.h"

class CAnimation : public CResource
{
public:
	CAnimation(CGame *game, unsigned int handle, const std::string &name);
	~CAnimation();

	const AnimationState &GetInitialState() const { return m_animation.states.at(m_animation.initialState); }
	const AnimationState &GetState(const std::string &name) const { return m_animation.states.at(name); }
	const AnimationFrame &GetFrame(const std::string &name, unsigned int index) const { return m_animation.states.at(name).frames[index]; }
	unsigned int GetStateCount() const { return m_animation.states.size(); }
	unsigned int GetFrameCount(const std::string &name) const { return m_animation.states.at(name).frames.size(); }
	void GetStateNames(std::vector<std::string> &names) const;

	static const std::string &GetResourcePath() { return m_resourcePath; }

private:
	Animation m_animation;

	static const std::string m_resourcePath;
};

#endif