#include "CGeometry.h"
#include "Game/CGame.h"

CGeometry::~CGeometry()
{
	if (m_game != nullptr)
		m_game->GetGeometryManager().DestroyGeometry(m_handle);
}