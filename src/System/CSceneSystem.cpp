#include "CSceneSystem.h"
#include "Game/CGame.h"

CSceneSystem::~CSceneSystem()
{
	if (m_initialized)
		m_game->GetGameLoop().UnregisterSystem(*this);
}

bool CSceneSystem::Initialize(CGame *game)
{
	if (ISystem::Initialize(game))
	{
		m_game->GetGameLoop().RegisterSystem(*this);

		m_initialized = true;
	}

	return m_initialized;
}

void CSceneSystem::Add(CObject &object)
{
	bool cameraDescendant = (object.GetType() == ECObjectType::Camera);

	std::shared_ptr<CObject> child = object.GetChild();

	while (child != nullptr)
	{
		if (child->GetType() == ECObjectType::Mesh)
		{
			unsigned int nodeIndex = (cameraDescendant ? UINT_MAX : m_scenePartitionTree->FindLeafNode(child->GetX(), child->GetZ()));
			m_scenePartitionTree->AddSceneEntity(nodeIndex, CSceneEntity(m_game, child, cameraDescendant));
		}

		child = child->GetChild();
	}
}

void CSceneSystem::Add(std::shared_ptr<CObject> object)
{
	if (object->GetParent() != nullptr)
	{
		if (object->GetType() == ECObjectType::Mesh)
		{
			bool cameraDescendant = false;

			CObject *parent = object->GetParent();

			while (parent->GetParent() != nullptr)
				parent = parent->GetParent();

			cameraDescendant = (parent->GetType() == ECObjectType::Camera);

			unsigned int nodeIndex = (cameraDescendant ? UINT_MAX : m_scenePartitionTree->FindLeafNode(object->GetX(), object->GetZ()));
			m_scenePartitionTree->AddSceneEntity(nodeIndex, CSceneEntity(m_game, std::move(object), cameraDescendant));
		}
	}
	else
	{
		if (object->GetType() == ECObjectType::Mesh)
		{
			unsigned int nodeIndex = m_scenePartitionTree->FindLeafNode(object->GetX(), object->GetZ());
			m_scenePartitionTree->AddSceneEntity(nodeIndex, CSceneEntity(m_game, object));
		}

		bool cameraDescendant = (object->GetType() == ECObjectType::Camera);

		std::shared_ptr<CObject> child = object->GetChild();

		while (child != nullptr)
		{
			if (child->GetType() == ECObjectType::Mesh)
			{
				unsigned int nodeIndex = (cameraDescendant ? UINT_MAX : m_scenePartitionTree->FindLeafNode(child->GetX(), child->GetZ()));
				m_scenePartitionTree->AddSceneEntity(nodeIndex, CSceneEntity(m_game, child, cameraDescendant));
			}

			child = child->GetChild();
		}
	}
}

bool CSceneSystem::Update()
{
	if (m_scenePartitionTree == nullptr)
		return false;

	std::vector<unsigned int> nodeIndices;

	m_scenePartitionTree->FindLeafNodes(m_game->GetCameraSystem().GetViewCamera(), nodeIndices);

	for (unsigned int i = 0; i < nodeIndices.size(); i++)
	{
		std::vector<CSceneEntity> &sceneEntities = *m_scenePartitionTree->GetSceneEntities(nodeIndices[i]);

		for (unsigned int j = 0; j < sceneEntities.size(); j++)
		{
			sceneEntities[j].SetVisible(true);
		}
	}

	return false;
}

bool CSceneSystem::CreateSceneEntity(std::shared_ptr<CObject> object, bool cameraDescendant, unsigned int &handle)
{
	SceneEntityData sceneEntity;
	sceneEntity.object = std::move(object);
	sceneEntity.cameraDescendant = cameraDescendant;
	sceneEntity.visible = cameraDescendant;

	handle = m_sceneEntityPool.Add(sceneEntity);
	m_sceneEntityHandles.push_back(handle);

	m_game->GetRenderSystem().UpdateScene(true);

	return true;
}

void CSceneSystem::DestroySceneEntity(unsigned int handle)
{
	m_sceneEntityHandles.remove(handle);
	m_sceneEntityPool.Remove(handle);

	m_game->GetRenderSystem().UpdateScene(true);
}

SceneEntityData &CSceneSystem::GetSceneEntity(unsigned int handle)
{
	return m_sceneEntityPool.Get(handle);
}