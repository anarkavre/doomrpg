#ifndef __CGEOMETRYMANAGER_H__
#define __CGEOMETRYMANAGER_H__

#include <memory>
#include <string>

#include "Geometry/CGeometry.h"
#include "Pool/CPool.h"

class CGame;

class CGeometryManager
{
public:
	CGeometryManager(CGame *game = nullptr) : m_initialized(false) { Initialize(game); }

	bool Initialize(CGame *game);

	template <class T> std::shared_ptr<T> AcquireGeometry(unsigned int id, const std::string &name);
	std::shared_ptr<CGeometry> GetGeometry(unsigned int id, const std::string &name) const;
	void DestroyGeometry(unsigned int handle);

	bool IsInitialized() const { return m_initialized; }

private:
	bool m_initialized;
	CGame *m_game;
	CPool<std::weak_ptr<CGeometry>> m_geometryPool;
};


template <class T>
std::shared_ptr<T> CGeometryManager::AcquireGeometry(unsigned int id, const std::string &name)
{
	if (!m_initialized || name.empty())
		return nullptr;

	std::shared_ptr<CGeometry> geometry = GetGeometry(id, name);

	if (geometry != nullptr)
		return std::static_pointer_cast<T>(geometry);

	unsigned int handle = m_geometryPool.Add(std::weak_ptr<CGeometry>());

	geometry = std::shared_ptr<T>(std::make_shared<T>(m_game, handle, id, name));

	m_geometryPool.Set(handle, geometry);

	return std::static_pointer_cast<T>(geometry);
}

#endif