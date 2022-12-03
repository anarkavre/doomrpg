#ifndef __CRESOURCEMANAGER_H__
#define __CRESOURCEMANAGER_H__

#include <memory>
#include <string>

#include "Pool/CPool.h"
#include "Resource/CResource.h"

class CGame;

class CResourceManager
{
public:
	CResourceManager(CGame *game = nullptr) : m_initialized(false), m_basePath("data/") { Initialize(game); }

	bool Initialize(CGame *game);

	template <class T> std::shared_ptr<T> AcquireResource(const std::string &name);
	std::shared_ptr<CResource> GetResource(const std::string &name, const std::string &path) const;
	void GetResources(ECResourceType type, std::vector<std::shared_ptr<CResource>> &resources) const;
	void DestroyResource(unsigned int handle);

	const std::string &GetBasePath() const { return m_basePath; }
	void SetBasePath(const std::string &basePath) { m_basePath = basePath; }

	bool IsInitialized() const { return m_initialized; }

private:
	bool m_initialized;
	CGame *m_game;
	std::string m_basePath;
	CPool<std::weak_ptr<CResource>> m_resourcePool;
};

template <class T>
std::shared_ptr<T> CResourceManager::AcquireResource(const std::string &name)
{
	if (!m_initialized || name.empty())
		return nullptr;

	std::shared_ptr<CResource> resource = GetResource(name, T::GetResourcePath());

	if (resource != nullptr)
		return std::static_pointer_cast<T>(resource);

	unsigned int handle = m_resourcePool.Add(std::weak_ptr<CResource>());

	try
	{
		resource = std::shared_ptr<T>(std::make_shared<T>(m_game, handle, name));

		m_resourcePool.Set(handle, resource);
	}
	catch (const std::exception &e)
	{
		m_resourcePool.Remove(handle);

		throw e;
	}

	return std::static_pointer_cast<T>(resource);
}

#endif