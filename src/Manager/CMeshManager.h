#ifndef __CMESHMANAGER_H__
#define __CMESHMANAGER_H__

#include <memory>
#include <string>

#include "Mesh/CMesh.h"
#include "Pool/CPool.h"

class CGame;

class CMeshManager
{
public:
	CMeshManager(CGame *game = nullptr) : m_initialized(false) { Initialize(game); }

	bool Initialize(CGame *game);

	template <class T> std::shared_ptr<T> AcquireMesh(unsigned int id, const std::string &name);
	template <class T> std::shared_ptr<T> AcquireMeshInstanced(unsigned int id, const std::string &name);
	std::shared_ptr<CMesh> GetMesh(unsigned int id, const std::string &name) const;
	void DestroyMesh(unsigned int handle);

	bool IsInitialized() const { return m_initialized; }

private:
	bool m_initialized;
	CGame *m_game;
	CPool<std::weak_ptr<CMesh>> m_meshPool;
};


template <class T>
std::shared_ptr<T> CMeshManager::AcquireMesh(unsigned int id, const std::string &name)
{
	if (!m_initialized || name.empty())
		return nullptr;

	std::shared_ptr<CMesh> mesh = GetMesh(id, name);

	if (mesh != nullptr)
		return std::static_pointer_cast<T>(mesh);

	unsigned int handle = m_meshPool.Add(std::weak_ptr<CMesh>());

	mesh = std::shared_ptr<T>(std::make_shared<T>(m_game, handle, id, name));

	m_meshPool.Set(handle, mesh);

	return std::static_pointer_cast<T>(mesh);
}

template <class T>
std::shared_ptr<T> CMeshManager::AcquireMeshInstanced(unsigned int id, const std::string &name)
{
	if (!m_initialized || name.empty())
		return nullptr;

	std::shared_ptr<CMesh> mesh = GetMesh(id, name);

	unsigned int handle = m_meshPool.Add(std::weak_ptr<CMesh>());

	if (mesh != nullptr)
		mesh = std::shared_ptr<T>(std::make_shared<T>(handle, *static_cast<T *>(mesh.get())));
	else
		mesh = std::shared_ptr<T>(std::make_shared<T>(m_game, handle, id, name));

	m_meshPool.Set(handle, mesh);

	return std::static_pointer_cast<T>(mesh);
}

#endif