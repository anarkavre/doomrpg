#include "CSceneEntity.h"
#include "Game/CGame.h"
#include "Mesh/CMesh.h"
#include "System/CSceneSystem.h"

CSceneEntity::~CSceneEntity()
{
	if (m_initialized)
		m_game->GetSceneSystem().DestroySceneEntity(m_handle);
}

bool CSceneEntity::Initialize(CGame *game, std::shared_ptr<CObject> object, bool cameraDescendant)
{
	if (game != nullptr)
	{
		m_game = game;

		m_game->GetSceneSystem().CreateSceneEntity(std::move(object), cameraDescendant, m_handle);

		m_initialized = true;
	}

	return m_initialized;
}

bool CSceneEntity::GetVisible() const
{
	SceneEntityData &sceneEntity = m_game->GetSceneSystem().GetSceneEntity(m_handle);
	return sceneEntity.visible;
}

void CSceneEntity::SetVisible(bool visible)
{
	SceneEntityData &sceneEntity = m_game->GetSceneSystem().GetSceneEntity(m_handle);
	sceneEntity.visible = visible;
}

std::shared_ptr<CObject> CSceneEntity::GetObject() const
{
	SceneEntityData &sceneEntity = m_game->GetSceneSystem().GetSceneEntity(m_handle);
	return sceneEntity.object;
}