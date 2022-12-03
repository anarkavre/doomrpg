#include "CAnimatedQuadList.h"

CAnimatedQuadList::CAnimatedQuadList(CGame *game, unsigned int handle, unsigned int id, const std::string &name) : CGeometry(game, handle, id, name)
{
	m_vertexComponents = { ECVertexComponent::Position3D, ECVertexComponent::TexCoord3D };

	m_vertexData.reserve(sizeof(AnimatedQuadVertexData) / sizeof(float));
}

void CAnimatedQuadList::Add(const AnimatedQuad &animatedQuad)
{
	unsigned int offset = m_vertexData.size();

	m_vertexData.resize(m_vertexData.size() + 4 * sizeof(AnimatedQuadVertexData) / sizeof(float));

	AnimatedQuadVertexData *vertexData = reinterpret_cast<AnimatedQuadVertexData *>(m_vertexData.data() + offset);

	const AnimatedQuadVertex &vertex1 = animatedQuad.vertices[0];
	const AnimatedQuadVertex &vertex2 = animatedQuad.vertices[1];
	const AnimatedQuadVertex &vertex3 = animatedQuad.vertices[2];
	const AnimatedQuadVertex &vertex4 = animatedQuad.vertices[3];

	float x1 = vertex1.position.x;
	float y1 = vertex1.position.y;
	float z1 = vertex1.position.z;
	float u1 = vertex1.texCoord.u;
	float v1 = vertex1.texCoord.v;
	float x2 = vertex2.position.x;
	float y2 = vertex2.position.y;
	float z2 = vertex2.position.z;
	float u2 = vertex2.texCoord.u;
	float v2 = vertex2.texCoord.v;
	float x3 = vertex3.position.x;
	float y3 = vertex3.position.y;
	float z3 = vertex3.position.z;
	float u3 = vertex3.texCoord.u;
	float v3 = vertex3.texCoord.v;
	float x4 = vertex4.position.x;
	float y4 = vertex4.position.y;
	float z4 = vertex4.position.z;
	float u4 = vertex4.texCoord.u;
	float v4 = vertex4.texCoord.v;
	float w = static_cast<float>(animatedQuad.textureIndex);

	vertexData[0] = { x1, y1, z1, u1, v1, w };
	vertexData[1] = { x2, y2, z2, u2, v2, w };
	vertexData[2] = { x3, y3, z3, u3, v3, w };
	vertexData[3] = { x4, y4, z4, u4, v4, w };

	unsigned int start = m_vertexCount;

	std::vector<unsigned int> indexData{ start + 0, start + 1, start + 2, start + 2, start + 1, start + 3 };

	m_indexData.insert(m_indexData.end(), indexData.begin(), indexData.end());

	m_vertexCount += 4;
}

void CAnimatedQuadList::Add(unsigned int textureIndex, const glm::vec3 &offset)
{
	float x1 = -0.5f + offset.x;
	float y1 = 0.5f + offset.y;
	float z1 = 0.0f + offset.z;
	float x2 = 0.5f + offset.x;
	float y2 = -0.5f + offset.y;
	float z2 = 0.0f + offset.z;
	float u1 = 0.0f;
	float v1 = 0.0f;
	float u2 = 1.0f;
	float v2 = 1.0f;

	AnimatedQuad animatedQuad;
	animatedQuad.vertices[0] = { x1, y1, z1, u1, v1 };
	animatedQuad.vertices[1] = { x1, y2, z1, u1, v2 };
	animatedQuad.vertices[2] = { x2, y1, z2, u2, v1 };
	animatedQuad.vertices[3] = { x2, y2, z2, u2, v2 };
	animatedQuad.textureIndex = textureIndex;

	Add(animatedQuad);
}