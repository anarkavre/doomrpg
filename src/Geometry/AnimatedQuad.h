#ifndef __ANIMATEDQUAD_H__
#define __ANIMATEDQUAD_H__

#include "glm/glm.hpp"

#include "Renderer/VertexComponents.h"

struct AnimatedQuadVertexData
{
	VertexComponent::Position3D position;
	VertexComponent::TexCoord3D texCoord;
};

struct AnimatedQuadVertex
{
	VertexComponent::Position3D position;
	VertexComponent::TexCoord2D texCoord;
};

struct AnimatedQuad
{
	AnimatedQuadVertex vertices[4];
	unsigned int textureIndex;
};

#include "CAnimatedQuad.h"
#include "CAnimatedQuadList.h"

#endif