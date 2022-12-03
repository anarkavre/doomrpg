#ifndef __CCOLLISIONHANDLER_H__
#define __CCOLLISIONHANDLER_H__

#include <memory>

#include "Interface/ICollisionHandler.h"

class CMap;

class CCollisionHandler : public ICollisionHandler
{
public:
	CCollisionHandler(std::shared_ptr<CMap> map = std::shared_ptr<CMap>()) : m_map(std::move(map)) {}

	void HandleCollision(CObject *object);

	std::shared_ptr<CMap> GetMap() const { return m_map; }
	void SetMap(std::shared_ptr<CMap> map) { m_map = std::move(map); }

private:
	std::shared_ptr<CMap> m_map;
};

#endif