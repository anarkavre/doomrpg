#include "CBSPTree.h"
#include "Camera/CCamera.h"

void CBSPTree::Clear()
{
	m_nodes.clear();
	m_leafNodes.clear();
	m_cameraEntities.clear();
}

unsigned int CBSPTree::AddNode(const BSPNode &node)
{
	unsigned int nodeIndex = m_nodes.size();

	m_nodes.push_back(node);

	if (node.type == ECBSPNodeType::Leaf)
		m_leafNodes.insert(std::make_pair(nodeIndex, BSPLeafNode{}));

	return nodeIndex;
}

unsigned int CBSPTree::FindLeafNode(float x, float y, unsigned int nodeIndex)
{
	BSPNode &node = m_nodes[nodeIndex];

	if (node.type == ECBSPNodeType::Leaf)
		return nodeIndex;
	else if (node.type == ECBSPNodeType::VerticalSplit)
	{
		if (x < node.splitPosition)
			return FindLeafNode(x, y, node.left);
		else
			return FindLeafNode(x, y, node.right);
	}
	else if (node.type == ECBSPNodeType::HorizontalSplit)
	{
		if (y < node.splitPosition)
			return FindLeafNode(x, y, node.left);
		else
			return FindLeafNode(x, y, node.right);
	}

	return 0;
}

void CBSPTree::FindLeafNodes(const CCamera &camera, std::vector<unsigned int> &nodeIndices, unsigned int nodeIndex)
{
	BSPNode &node = m_nodes[nodeIndex];

	if (camera.IsBoxInFrustum(node.bboxMin, node.bboxMax))
	{
		if (node.type == ECBSPNodeType::Leaf)
			nodeIndices.push_back(nodeIndex);
		else
		{
			FindLeafNodes(camera, nodeIndices, node.left);
			FindLeafNodes(camera, nodeIndices, node.right);
		}
	}
}

bool CBSPTree::AddSceneEntity(unsigned int nodeIndex, CSceneEntity &sceneEntity)
{
	if (nodeIndex == UINT_MAX)
	{
		sceneEntity.SetVisible(true);

		m_cameraEntities.push_back(std::move(sceneEntity));

		return true;
	}
	else if (m_nodes[nodeIndex].type == ECBSPNodeType::Leaf)
	{
		m_leafNodes[nodeIndex].sceneEntities.push_back(std::move(sceneEntity));

		return true;
	}
	else
		return false;
}