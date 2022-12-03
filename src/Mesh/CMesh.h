#ifndef __CMESH_H__
#define __CMESH_H__

#include <memory>
#include <string>
#include <vector>

#include "Geometry/CGeometry.h"
#include "Material/CMaterial.h"
#include "Object/CObject.h"
#include "Renderer/VertexComponents.h"

class CGame;

class CMesh : public CObject
{
public:
	CMesh(unsigned int handle, const CMesh &mesh) : CObject(ECObjectType::Mesh), m_initialized(false), m_game(mesh.m_game), m_handle(handle), m_id(mesh.m_id), m_name(mesh.m_name) { Initialize(mesh.m_geometry, mesh.m_material, mesh.m_instanced, mesh.m_billboard); }
	CMesh(CGame *game, unsigned int handle, unsigned int id, const std::string &name) : CObject(ECObjectType::Mesh), m_initialized(false), m_game(game), m_handle(handle), m_id(id), m_name(name), m_instanced(false), m_billboard(false) {}
	~CMesh();

	bool Initialize(std::shared_ptr<CGeometry> geometry, std::shared_ptr<CMaterial> material, bool instanced, bool billboard);
	virtual const std::vector<unsigned int> &GetIndexData() const { return m_geometry->GetIndexData(); }
	virtual void SetUniforms() {}

	unsigned int GetID() const { return m_id; }
	const std::string &GetName() const { return m_name; }
	CGeometry *GetGeometry() const { return m_geometry.get(); }
	std::shared_ptr<CMaterial> GetMaterial() const { return m_material; }

	bool IsInitialized() const { return m_initialized; }
	bool IsInstanced() const { return m_instanced; }
	bool IsBillboard() const { return m_billboard; }

protected:
	bool m_initialized;
	CGame *m_game;

private:
	unsigned int m_handle;
	unsigned int m_id;
	std::string m_name;
	std::shared_ptr<CGeometry> m_geometry;
	std::shared_ptr<CMaterial> m_material;
	bool m_instanced;
	bool m_billboard;
};

#endif