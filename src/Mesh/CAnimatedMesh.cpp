#include "CAnimatedMesh.h"
#include "Game/CGame.h"
#include "Resource/CAnimation.h"
#include "Resource/CShader.h"

CAnimatedMesh::CAnimatedMesh(unsigned int handle, const CAnimatedMesh &animatedMesh) : CMesh(handle, animatedMesh)
{
	const std::string &name = animatedMesh.m_textureAnimation.GetAnimation()->GetName();
	std::string animation = name.substr(0, name.find_last_of('.'));
	m_textureAnimation.Initialize(m_game, animation);
}

bool CAnimatedMesh::Initialize(const std::string &animation, std::shared_ptr<CGeometry> geometry, std::shared_ptr<CMaterial> material, bool instanced, bool billboard)
{
	if (!IsInitialized())
	{
		m_textureAnimation.Initialize(m_game, animation);

		material->SetShader(m_game->GetResourceManager().AcquireResource<CShader>("animated_quad.shader"));

		return CMesh::Initialize(std::move(geometry), std::move(material), instanced, billboard);
	}

	return true;
}

void CAnimatedMesh::SetUniforms()
{
	GetMaterial()->GetShader()->SetUniform("frame", static_cast<float>(m_textureAnimation.GetFrame()));
}