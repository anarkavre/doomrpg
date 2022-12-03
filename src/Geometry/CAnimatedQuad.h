#ifndef __CANIMATEDQUAD_H__
#define __CANIMATEDQUAD_H__

#include "AnimatedQuad.h"
#include "CGeometry.h"

class CAnimatedQuad : public CGeometry
{
public:
	CAnimatedQuad(CGame *game, unsigned int handle, unsigned int id, const std::string &name);

	void SetTextureIndex(unsigned int textureIndex);
};

#endif