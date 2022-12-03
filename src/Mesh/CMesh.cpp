#include "CMesh.h"
#include "Game/CGame.h"

CMesh::~CMesh()
{
	if (m_game != nullptr)
		m_game->GetMeshManager().DestroyMesh(m_handle);
}

bool CMesh::Initialize(std::shared_ptr<CGeometry> geometry, const std::shared_ptr<CMaterial> material, bool instanced, bool billboard)
{
	if (!m_initialized)
	{
		m_geometry = std::move(geometry);
		m_material = std::move(material);
		m_instanced = instanced;
		m_billboard = billboard;
		m_initialized = true;
	}

	return m_initialized;
}