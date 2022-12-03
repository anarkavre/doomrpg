#ifndef __CANIMATEDMESHSTATEFUL_H__
#define __CANIMATEDMESHSTATEFUL_H__

#include "CMesh.h"
#include "Animation/CTextureAnimation.h"

struct AnimatedMeshStatefulData
{
	std::vector<unsigned int> indexData;
	std::vector<CTextureAnimation> textureAnimations;
};
class CAnimatedMeshStateful : public CMesh
{
public:
	CAnimatedMeshStateful(unsigned int handle, const CAnimatedMeshStateful &animatedMeshStateful) : CMesh(handle, animatedMeshStateful), m_data(animatedMeshStateful.m_data), m_state(0) {}
	CAnimatedMeshStateful(CGame *game, unsigned int handle, unsigned int id, const std::string &name) : CMesh(game, handle, id, name), m_state(0) {}

	bool Initialize(std::shared_ptr<CGeometry> geometry, std::shared_ptr<CMaterial> material, bool instanced = false, bool billboard = false);
	void AddState(const std::string &animation);
	const std::vector<unsigned int> &GetIndexData() const { return m_data->indexData; }
	void SetUniforms();

	unsigned int GetState() const { return m_state; }
	void SetState(unsigned int state) { m_state = state; ChangeIndexData(); }
	CTextureAnimation &GetTextureAnimation() const { return m_data->textureAnimations[m_state]; }

private:
	void ChangeIndexData();

	std::shared_ptr<AnimatedMeshStatefulData> m_data;
	unsigned int m_state;
};

#endif