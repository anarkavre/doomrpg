#include "CResourceManager.h"

bool CResourceManager::Initialize(CGame *game)
{
	if (!m_initialized && game != nullptr)
	{
		m_game = game;
		m_initialized = true;
	}

	return m_initialized;
}

std::shared_ptr<CResource> CResourceManager::GetResource(const std::string &name, const std::string &path) const
{
	if (!m_initialized || m_resourcePool.IsEmpty())
		return nullptr;

	for (auto it = m_resourcePool.GetPool().cbegin(); it != m_resourcePool.GetPool().cend(); ++it)
	{
		if (!((*it).expired()))
		{
			std::shared_ptr<CResource> resource = (*it).lock();

			if (resource->GetName() == name && resource->GetPath() == m_basePath + path)
				return resource;
		}
	}

	return nullptr;
}

void CResourceManager::GetResources(ECResourceType type, std::vector<std::shared_ptr<CResource>> &resources) const
{
	if (!m_initialized || m_resourcePool.IsEmpty())
		return;

	for (auto it = m_resourcePool.GetPool().cbegin(); it != m_resourcePool.GetPool().cend(); ++it)
	{
		if (!((*it).expired()))
		{
			std::shared_ptr<CResource> resource = (*it).lock();

			if (resource->GetType() == type)
				resources.push_back(resource);
		}
	}
}

void CResourceManager::DestroyResource(unsigned int handle)
{
	if (!m_initialized || m_resourcePool.IsEmpty())
		return;

	m_resourcePool.Remove(handle);
}