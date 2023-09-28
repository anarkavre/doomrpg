#include "CGeometryManager.h"

bool CGeometryManager::Initialize(CGame *game)
{
	if (!m_initialized && game != nullptr)
	{
		m_game = game;
		m_initialized = true;
	}

	return m_initialized;
}

std::shared_ptr<CGeometry> CGeometryManager::GetGeometry(unsigned int id, const std::string &name) const
{
	if (!m_initialized || m_geometryPool.IsEmpty())
		return nullptr;

	for (auto it = m_geometryPool.GetPool().cbegin(); it != m_geometryPool.GetPool().cend(); ++it)
	{
		if (!((*it).expired()))
		{
			std::shared_ptr<CGeometry> geometry = (*it).lock();

			if (geometry->GetID() == id && geometry->GetName() == name)
				return geometry;
		}
	}

	return nullptr;
}

void CGeometryManager::DestroyGeometry(unsigned int handle)
{
	if (!m_initialized || m_geometryPool.IsEmpty())
		return;

	m_geometryPool.Remove(handle);
}