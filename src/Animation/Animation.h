#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <string>
#include <unordered_map>
#include <vector>

struct AnimationFrame
{
	unsigned int frame;
	float duration;
	int horizontalOffset;
	int verticalOffset;
	std::string sound;
};

struct AnimationState
{
	std::string name;
	std::vector<AnimationFrame> frames;
	std::string next;
	std::string sound;
	unsigned int loops;
};

struct Animation
{
	std::string initialState;
	std::unordered_map<std::string, AnimationState> states;
};

#include "CTextureAnimation.h"

#endif