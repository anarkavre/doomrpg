#ifndef __CSCENEPARTITIONTREE_H__
#define __CSCENEPARTITIONTREE_H__

#include <vector>

#include "glm/glm.hpp"

#include "CSceneEntity.h"

class CCamera;

class CScenePartitionTree
{
public:
	virtual void Clear() {}
	virtual unsigned int FindLeafNode(float x, float y, unsigned int nodeIndex = 0) { return 0; }
	virtual void FindLeafNodes(const CCamera &camera, std::vector<unsigned int> &nodeIndices, unsigned int nodeIndex = 0) {}
	virtual bool AddSceneEntity(unsigned int nodeIndex, CSceneEntity &sceneEntity) { return false; }

	virtual std::vector<CSceneEntity> *GetSceneEntities(unsigned int nodeIndex) { return nullptr; }
};
#endif