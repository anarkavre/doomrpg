#include "CAnimatedQuad.h"

CAnimatedQuad::CAnimatedQuad(CGame *game, unsigned int handle, unsigned int id, const std::string &name) : CGeometry(game, handle, id, name)
{
	m_vertexComponents = { ECVertexComponent::Position3D, ECVertexComponent::TexCoord3D };

	m_vertexData.resize(4 * sizeof(AnimatedQuadVertexData) / sizeof(float));

	AnimatedQuadVertexData *vertexData = reinterpret_cast<AnimatedQuadVertexData *>(m_vertexData.data());

	float x1 = -0.5f;
	float y1 = 0.5f;
	float z1 = 0.0f;
	float x2 = 0.5f;
	float y2 = -0.5f;
	float z2 = 0.0f;
	float u1 = 0.0f;
	float v1 = 0.0f;
	float u2 = 1.0f;
	float v2 = 1.0f;
	float w = 0.0f;

	vertexData[0] = { x1, y1, z1, u1, v1, w };
	vertexData[1] = { x1, y2, z1, u1, v2, w };
	vertexData[2] = { x2, y1, z2, u2, v1, w };
	vertexData[3] = { x2, y2, z2, u2, v2, w };

	m_indexData = { 0, 1, 2, 2, 1, 3 };

	m_vertexCount = 4;
}

void CAnimatedQuad::SetTextureIndex(unsigned int textureIndex)
{
	AnimatedQuadVertexData *vertexData = reinterpret_cast<AnimatedQuadVertexData *>(m_vertexData.data());

	float w = static_cast<float>(textureIndex);

	vertexData[0].texCoord.w = w;
	vertexData[1].texCoord.w = w;
	vertexData[2].texCoord.w = w;
	vertexData[3].texCoord.w = w;
}