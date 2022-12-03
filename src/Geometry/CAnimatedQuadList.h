#ifndef __CANIMATEDQUADLIST_H__
#define __CANIMATEDQUADLIST_H__

#include "AnimatedQuad.h"
#include "CGeometry.h"

class CAnimatedQuadList : public CGeometry
{
public:
	CAnimatedQuadList(CGame *game, unsigned int handle, unsigned int id, const std::string &name);

	void Add(const AnimatedQuad &animatedQuad);
	void Add(unsigned int textureIndex, const glm::vec3 &offset = glm::vec3(0.0f));
};

#endif