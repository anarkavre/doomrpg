#ifndef __CANIMATEDMESH_H__
#define __CANIMATEDMESH_H__

#include "CMesh.h"
#include "Animation/CTextureAnimation.h"

class CAnimatedMesh : public CMesh
{
public:
	CAnimatedMesh(unsigned int handle, const CAnimatedMesh &animatedMesh);
	CAnimatedMesh(CGame *game, unsigned int handle, unsigned int id, const std::string &name) : CMesh(game, handle, id, name) {}

	bool Initialize(const std::string &animation, std::shared_ptr<CGeometry> geometry, std::shared_ptr<CMaterial> material, bool instanced = false, bool billboard = false);
	void SetUniforms();

private:
	CTextureAnimation m_textureAnimation;
};

#endif