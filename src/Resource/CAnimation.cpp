#include "CAnimation.h"
#include "Game/CGame.h"
#include "Utility/JSON/JSON.h"

CAnimation::CAnimation(CGame *game, unsigned int handle, const std::string &name) : CResource(game, handle, name, GetResourcePath(), ECResourceType::Animation)
{
	unsigned char *data = new unsigned char[GetSize()];
	std::string text(reinterpret_cast<char *>(GetData(data, GetSize())), GetSize());
	delete[] data;

	JSON::Parse<Animation>(text, m_animation);

	for (auto it = m_animation.states.begin(); it != m_animation.states.end(); ++it)
		(*it).second.name = (*it).first;
}

CAnimation::~CAnimation() {}

void CAnimation::GetStateNames(std::vector<std::string> &names) const
{
	for (auto it = m_animation.states.cbegin(); it != m_animation.states.cend(); ++it)
		names.push_back((*it).first);
}

const std::string CAnimation::m_resourcePath = "animations/";