#include "CThing.h"
#include "Geometry/CAnimatedQuad.h"
#include "Geometry/CAnimatedQuadList.h"
#include "Mesh/CAnimatedMesh.h"

CThing::CThing(CDoomRPG *doomrpg, const thing_t *thing) : CEntity(doomrpg, thing->id)
{
	SetX(thing->position.x * 8 / 64.0f);
	SetY(thing->id == ENTITY_LIGHT_2 ? 0.078125f : 0.0f);
	SetZ(thing->position.y * 8 / 64.0f);
	SetHidden(thing->flags & TF_HIDDEN);
	UpdateTransformationMatrix();

	if (GetId() != ENTITY_PISTOL && (GetId() & 0xD0) != 0xD0)
	{
		if (GetType() == ENTITY_TYPE_AMMO_MULTIPLE)
		{
			std::shared_ptr<CAnimatedMesh> animatedMesh = m_doomrpg->GetMeshManager().AcquireMeshInstanced<CAnimatedMesh>(GetId(), "entity_multiple_" + std::to_string(GetId()));

			if (!animatedMesh->IsInitialized())
			{
				unsigned int textureIndex = m_doomrpg->GetMappings()->thingMappings[GetId() - 1];

				std::shared_ptr<CAnimatedQuadList> animatedQuadList = m_doomrpg->GetGeometryManager().AcquireGeometry<CAnimatedQuadList>(GetId(), "entity_multiple_" + std::to_string(GetId()));
				animatedQuadList->Add(textureIndex, glm::vec3(0.0f, 0.0f, -0.25f));
				animatedQuadList->Add(textureIndex, glm::vec3(0.0f, 0.0f, 0.25f));
				animatedQuadList->Add(textureIndex, glm::vec3(-0.25f, 0.0f, 0.0f));
				animatedQuadList->Add(textureIndex, glm::vec3(0.25f, 0.0f, 0.0f));

				std::shared_ptr<CMaterial> material = m_doomrpg->GetMaterialManager().AcquireMaterial(7, "sprite");
				material->SetTexture(m_doomrpg->GetSprites());

				animatedMesh->Initialize("common", std::move(animatedQuadList), std::move(material), true, true);
			}

			animatedMesh->SetPosition(GetPosition());
			animatedMesh->SetRotation(GetRotation());
			animatedMesh->SetHidden(GetHidden());
			animatedMesh->UpdateTransformationMatrix();

			m_doomrpg->GetSceneSystem().Add(animatedMesh);

			m_mesh = animatedMesh;
		}
		else
		{
			std::shared_ptr<CAnimatedMesh> animatedMesh = m_doomrpg->GetMeshManager().AcquireMeshInstanced<CAnimatedMesh>(GetId(), "entity_" + std::to_string(GetId()));

			if (!animatedMesh->IsInitialized())
			{
				std::string animation;

				if (GetType() == ENTITY_TYPE_ENEMY)
				{
					if (GetId() == ENTITY_PHANTOM || GetId() == ENTITY_LOST_SOUL || GetId() == ENTITY_NIGHTMARE)
						animation = "lostsoul";
					else
						animation = "enemy";
				}
				else if (GetType() == ENTITY_TYPE_HUMAN)
					animation = "human";
				else
					animation = "common";

				unsigned int textureIndex = m_doomrpg->GetMappings()->thingMappings[GetId()];

				std::shared_ptr<CAnimatedQuad> animatedQuad = m_doomrpg->GetGeometryManager().AcquireGeometry<CAnimatedQuad>(GetId(), "entity_" + std::to_string(GetId()));
				animatedQuad->SetTextureIndex(textureIndex);

				std::shared_ptr<CMaterial> material;

				if ((m_doomrpg->IsEx() && GetId() == ENTITY_BELPHEGOR) || GetId() == ENTITY_LIGHT_1 || GetId() == ENTITY_LIGHT_2)
				{
					material = m_doomrpg->GetMaterialManager().AcquireMaterial(8, "transparent_sprite");
					material->SetTexture(m_doomrpg->GetSprites());
					material->SetProperties(ECMaterialProperties::Transparent);
					material->SetBlendMode(ECBlendMode::Additive);
				}
				else
				{
					material = m_doomrpg->GetMaterialManager().AcquireMaterial(7, "sprite");
					material->SetTexture(m_doomrpg->GetSprites());
				}

				animatedMesh->Initialize(animation, std::move(animatedQuad), std::move(material), true, true);
			}

			animatedMesh->SetPosition(GetPosition());
			animatedMesh->SetRotation(GetRotation());
			animatedMesh->SetHidden(GetHidden());
			animatedMesh->UpdateTransformationMatrix();

			m_doomrpg->GetSceneSystem().Add(animatedMesh);

			m_mesh = animatedMesh;
		}
	}
}

void CThing::SetHidden(bool hidden)
{
	CEntity::SetHidden(hidden);

	if (m_mesh != nullptr)
		m_mesh->SetHidden(hidden);
}