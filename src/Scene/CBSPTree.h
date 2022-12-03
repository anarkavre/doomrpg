#ifndef __CBSPTREE_H__
#define __CBSPTREE_H__

#include <unordered_map>
#include <vector>

#include "glm/glm.hpp"

#include "CScenePartitionTree.h"

enum class ECBSPNodeType
{
	Leaf,
	VerticalSplit,
	HorizontalSplit
};

struct BSPNode
{
	ECBSPNodeType type;
	glm::vec3 bboxMin, bboxMax;
	float splitPosition;
	unsigned int left;
	unsigned int right;
};

struct BSPLeafNode
{
	std::vector<CSceneEntity> sceneEntities;
};

class CCamera;

class CBSPTree : public CScenePartitionTree
{
public:
	void Clear();
	unsigned int AddNode(const BSPNode &node);
	unsigned int FindLeafNode(float x, float y, unsigned int nodeIndex = 0);
	void FindLeafNodes(const CCamera &camera, std::vector<unsigned int> &nodeIndices, unsigned int nodeIndex = 0);
	bool AddSceneEntity(unsigned int nodeIndex, CSceneEntity &sceneEntity);

	const BSPLeafNode *GetLeafNode(unsigned int nodeIndex) const { return (m_leafNodes.count(nodeIndex) == 1 ? &m_leafNodes.at(nodeIndex) : nullptr); }
	std::vector<CSceneEntity> *GetSceneEntities(unsigned int nodeIndex) { return (m_leafNodes.count(nodeIndex) == 1 ? &m_leafNodes.at(nodeIndex).sceneEntities : nullptr); }

private:
	std::vector<BSPNode> m_nodes;
	std::unordered_map<unsigned int, BSPLeafNode> m_leafNodes;
	std::vector<CSceneEntity> m_cameraEntities;
};
#endif