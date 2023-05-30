#ifndef __CMATERIALMANAGER_H__
#define __CMATERIALMANAGER_H__

#include <memory>
#include <string>

#include "Material/CMaterial.h"
#include "Pool/CPool.h"

class CGame;

class CMaterialManager
{
public:
	CMaterialManager(CGame *game = nullptr) { Initialize(game); }

	bool Initialize(CGame *game);

	std::shared_ptr<CMaterial> AcquireMaterial(unsigned int id, const std::string &name);
	std::shared_ptr<CMaterial> GetMaterial(unsigned int id, const std::string &name) const;
	void DestroyMaterial(unsigned int handle);

	bool IsInitialized() const { return m_initialized; }

private:
	bool m_initialized{false};
	CGame *m_game{nullptr};
	CPool<std::weak_ptr<CMaterial>> m_materialPool;
};
#endif