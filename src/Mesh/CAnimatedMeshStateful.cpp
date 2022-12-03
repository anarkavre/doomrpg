#include "CAnimatedMeshStateful.h"
#include "Game/CGame.h"
#include "Resource/CAnimation.h"
#include "Resource/CShader.h"

bool CAnimatedMeshStateful::Initialize(std::shared_ptr<CGeometry> geometry, std::shared_ptr<CMaterial> material, bool instanced, bool billboard)
{
	if (!IsInitialized())
	{
		m_data = std::make_shared<AnimatedMeshStatefulData>();

		m_data->indexData.resize(6);

		ChangeIndexData();

		material->SetShader(m_game->GetResourceManager().AcquireResource<CShader>("animated_quad.shader"));

		return CMesh::Initialize(std::move(geometry), std::move(material), instanced, billboard);
	}

	return true;
}

void CAnimatedMeshStateful::AddState(const std::string &animation)
{
	m_data->textureAnimations.push_back(CTextureAnimation());
	m_data->textureAnimations.back().Initialize(m_game, animation);
}

void CAnimatedMeshStateful::SetUniforms()
{
	const CTextureAnimation &textureAnimation = m_data->textureAnimations[m_state];
	float horizontalOffset = textureAnimation.GetHorizontalOffset() / 64.0f;
	float verticalOffset = textureAnimation.GetVerticalOffset() / 64.0f;

	SetOffsetX(horizontalOffset);
	SetOffsetY(verticalOffset);

	GetMaterial()->GetShader()->SetUniform("frame", static_cast<float>(textureAnimation.GetFrame()));
}

void CAnimatedMeshStateful::ChangeIndexData()
{
	unsigned int start = m_state * 4;
	m_data->indexData = { start + 0, start + 1, start + 2, start + 2, start + 1, start + 3 };
}