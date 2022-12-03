#ifndef __CSCENE_H__
#define __CSCENE_H__

#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Interface/ISystem.h"
#include "Pool/CPool.h"
#include "Scene/CScenePartitionTree.h"

class CObject;

struct SceneEntityData
{
	std::shared_ptr<CObject> object;
	bool cameraDescendant;
	bool visible;
};

class CSceneSystem : public ISystem
{
public:
	CSceneSystem(CGame *game = nullptr) { Initialize(game); }
	~CSceneSystem();

	bool Initialize(CGame *game);
	void Add(CObject &object);
	void Add(std::shared_ptr<CObject> object);
	bool Update();

	bool CreateSceneEntity(std::shared_ptr<CObject> object, bool cameraDescent, unsigned int &handle);
	void DestroySceneEntity(unsigned int handle);
	SceneEntityData &GetSceneEntity(unsigned int handle);

	const std::list<unsigned int> &GetSceneEntityHandles() const { return m_sceneEntityHandles; }
	CScenePartitionTree &GetScenePartitionTree() { return *m_scenePartitionTree; }
	void SetScenePartitionTree(CScenePartitionTree &scenePartitionTree) { m_scenePartitionTree = &scenePartitionTree; }

private:
	CPool<SceneEntityData> m_sceneEntityPool;
	std::list<unsigned int> m_sceneEntityHandles;
	CScenePartitionTree *m_scenePartitionTree;
};

#endif