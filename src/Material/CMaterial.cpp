#include "CMaterial.h"
#include "Game/CGame.h"

CMaterial::~CMaterial()
{
	if (m_game != nullptr)
		m_game->GetMaterialManager().DestroyMaterial(m_handle);
}