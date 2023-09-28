#include "CMaterialManager.h"

bool CMaterialManager::Initialize(CGame *game)
{
	if (!m_initialized && game != nullptr)
	{
		m_game = game;
		m_initialized = true;
	}

	return m_initialized;
}

std::shared_ptr<CMaterial> CMaterialManager::AcquireMaterial(unsigned int id, const std::string &name)
{
	if (!m_initialized || name.empty())
		return nullptr;

	std::shared_ptr<CMaterial> material = GetMaterial(id, name);

	if (material != nullptr)
		return material;

	unsigned int handle = m_materialPool.Add(std::weak_ptr<CMaterial>());

	material = std::make_shared<CMaterial>(m_game, handle, id, name);

	m_materialPool.Set(handle, material);

	return material;
}

std::shared_ptr<CMaterial> CMaterialManager::GetMaterial(unsigned int id, const std::string &name) const
{
	if (!m_initialized || m_materialPool.IsEmpty())
		return nullptr;

	for (auto it = m_materialPool.GetPool().cbegin(); it != m_materialPool.GetPool().cend(); ++it)
	{
		if (!((*it).expired()))
		{
			std::shared_ptr<CMaterial> material = (*it).lock();

			if (material->GetID() == id && material->GetName() == name)
				return material;
		}
	}

	return nullptr;
}

void CMaterialManager::DestroyMaterial(unsigned int handle)
{
	if (!m_initialized || m_materialPool.IsEmpty())
		return;

	m_materialPool.Remove(handle);
}