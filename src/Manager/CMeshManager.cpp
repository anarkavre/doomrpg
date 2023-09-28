#include "CMeshManager.h"

bool CMeshManager::Initialize(CGame *game)
{
	if (!m_initialized && game != nullptr)
	{
		m_game = game;
		m_initialized = true;
	}

	return m_initialized;
}

std::shared_ptr<CMesh> CMeshManager::GetMesh(unsigned int id, const std::string &name) const
{
	if (!m_initialized || m_meshPool.IsEmpty())
		return nullptr;

	for (auto it = m_meshPool.GetPool().cbegin(); it != m_meshPool.GetPool().cend(); ++it)
	{
		if (!((*it).expired()))
		{
			std::shared_ptr<CMesh> mesh = (*it).lock();

			if (mesh->GetID() == id && mesh->GetName() == name)
				return mesh;
		}
	}

	return nullptr;
}

void CMeshManager::DestroyMesh(unsigned int handle)
{
	if (!m_initialized || m_meshPool.IsEmpty())
		return;

	m_meshPool.Remove(handle);
}