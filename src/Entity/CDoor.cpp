#include "CDoor.h"
#include "Geometry/CAnimatedQuad.h"
#include "Mesh/CAnimatedMesh.h"
#include "Resource/CSound.h"
#include "doomrpg_sounds.h"

CDoor::CDoor(CDoomRPG *doomrpg, const linesegment_t *line) : CEntity(doomrpg, ENTITY_DOOR + line->texture), m_open(false), m_state(m_doomrpg->GetEntity(ENTITY_DOOR + line->texture) != nullptr && m_doomrpg->GetEntity(ENTITY_DOOR + line->texture)->parameter1 == ENTITY_DOOR_LOCKED ? ECDoorState::Locked : ECDoorState::Closed), m_openDelayTics(0), m_closeDelayTics(0), m_speed(2.0f)
{
	float x1 = line->start.x * 8 / 64.0f;
	float z1 = line->start.y * 8 / 64.0f;
	float x2 = line->end.x * 8 / 64.0f;
	float z2 = line->end.y * 8 / 64.0f;
	
	if (line->flags & LF_SHIFT_EAST_OR_SOUTH)
	{
		if (line->flags & LF_WEST_FACING_WALL)
		{
			x1 += 0.046875f;
			x2 += 0.046875f;

			SetAngleY(270.0f);
		}
		else if (line->flags & LF_NORTH_FACING_WALL)
		{
			z1 += 0.046875f;
			z2 += 0.046875f;

			SetAngleY(180.0f);
		}
	}
	else if (line->flags & LF_SHIFT_WEST_OR_NORTH)
	{
		if (line->flags & LF_EAST_FACING_WALL)
		{
			x1 -= 0.046875f;
			x2 -= 0.046875f;

			SetAngleY(90.0f);
		}
		else if (line->flags & LF_SOUTH_FACING_WALL)
		{
			z1 -= 0.046875f;
			z2 -= 0.046875f;

			SetAngleY(0.0f);
		}
	}
	else
		SetAngleY(line->flags & LF_SHIFT_HORIZONTAL ? 0.0f : 270.0f);

	SetX(x1 + (x2 - x1) / 2);
	SetZ(z1 + (z2 - z1) / 2);
	SetHidden(false);
	SetState(line->flags & LF_LOCKED_DOOR ? ECDoorState::Locked : ECDoorState::Closed);

	std::shared_ptr<CAnimatedMesh> animatedMesh = m_doomrpg->GetMeshManager().AcquireMeshInstanced<CAnimatedMesh>(GetId(), "entity_" + std::to_string(GetId()));

	if (!animatedMesh->IsInitialized())
	{
		unsigned int textureIndex = m_doomrpg->GetMappings()->wallMappings[GetId() - ENTITY_DOOR];

		std::shared_ptr<CAnimatedQuad> animatedQuad = m_doomrpg->GetGeometryManager().AcquireGeometry<CAnimatedQuad>(GetId(), "entity_" + std::to_string(GetId()));
		animatedQuad->SetTextureIndex(textureIndex);

		std::shared_ptr<CMaterial> material;

		if (GetType() == 18)
		{
			material = m_doomrpg->GetMaterialManager().AcquireMaterial(6, "secret");
			material->SetTexture(m_doomrpg->GetTextures());
		}
		else
		{
			material = m_doomrpg->GetMaterialManager().AcquireMaterial(5, "door");
			material->SetTexture(m_doomrpg->GetTextures());
			material->SetProperties(ECMaterialProperties::DoubleSided);
		}

		animatedMesh->Initialize("common", std::move(animatedQuad), std::move(material), true);
	}

	animatedMesh->SetPosition(GetPosition());
	animatedMesh->SetRotation(GetRotation());
	animatedMesh->SetHidden(false);

	m_doomrpg->GetSceneSystem().Add(animatedMesh);

	m_mesh[0] = animatedMesh;

	if ((GetId() & 0xA) == 0xA)
	{
		animatedMesh = m_doomrpg->GetMeshManager().AcquireMeshInstanced<CAnimatedMesh>(GetId() ^ 0x1, "entity_" + std::to_string(GetId() ^ 0x1));

		if (!animatedMesh->IsInitialized())
		{
			unsigned int textureIndex = m_doomrpg->GetMappings()->wallMappings[(GetId() ^ 0x1) - ENTITY_DOOR];

			std::shared_ptr<CAnimatedQuad> animatedQuad = m_doomrpg->GetGeometryManager().AcquireGeometry<CAnimatedQuad>(GetId() ^ 0x1, "entity_" + std::to_string(GetId() ^ 0x1));
			animatedQuad->SetTextureIndex(textureIndex);

			std::shared_ptr<CMaterial> material = m_doomrpg->GetMaterialManager().AcquireMaterial(5, "door");
			material->SetTexture(m_doomrpg->GetTextures());
			material->SetProperties(ECMaterialProperties::DoubleSided);

			animatedMesh->Initialize("common", std::move(animatedQuad), std::move(material), true);
		}

		animatedMesh->SetPosition(GetPosition());
		animatedMesh->SetRotation(GetRotation());
		animatedMesh->SetHidden(true);

		m_doomrpg->GetSceneSystem().Add(animatedMesh);

		m_mesh[1] = animatedMesh;
	}

	m_sound[0] = doomrpg->GetResourceManager().AcquireResource<CSound>(std::string(GetType() == 0 ? SOUND_DOOR_OPEN : SOUND_SECRET) + ".wav");
	m_sound[1] = doomrpg->GetResourceManager().AcquireResource<CSound>(std::string(SOUND_DOOR_CLOSE) + ".wav");
}

void CDoor::Open()
{
	if (m_open)
		return;

	m_doomrpg->GetGameLoop().RegisterUpdateHandler(*this);

	m_tics = m_openDelayTics;

	m_open = true;
}

void CDoor::Close()
{
	if (!m_open)
		return;

	m_tics = m_closeDelayTics;

	m_open = false;
}

bool CDoor::Update(float elapsedTime)
{
	if (m_state == ECDoorState::Closed)
	{
		if (m_tics != 0)
			m_tics--;

		if (m_tics == 0)
		{
			m_state = ECDoorState::Opening;

			if (GetType() == 0 || GetType() == 18)
				m_sound[0]->Play();
		}
	}
	else if (m_state == ECDoorState::Opening)
	{
		if (GetType() == 18)
		{
			if (GetOffsetZ() > -0.90625)
			{
				SetOffsetZ(GetOffsetZ() - m_speed * elapsedTime);

				if (GetOffsetZ() < -0.90625)
					SetOffsetZ(-0.90625);
			}
			else
			{
				SetOffsetX(GetOffsetX() - m_speed * elapsedTime);

				if (GetOffsetX() < -1.0f)
				{
					m_state = ECDoorState::Opened;
					SetOffsetX(-1.0f);
					SetHidden(true);
				}
			}
		}
		else
		{
			SetOffsetX(GetOffsetX() + m_speed * elapsedTime);

			if (GetOffsetX() > 1.0f)
			{
				m_state = ECDoorState::Opened;
				SetOffsetX(1.0f);
				SetHidden(true);
			}
		}

		m_mesh[0]->SetOffsetX(GetOffsetX());
		m_mesh[0]->SetOffsetZ(GetOffsetZ());

		if (m_mesh[1] != nullptr)
			m_mesh[1]->SetOffsetX(GetOffsetX());
	}
	else if (m_state == ECDoorState::Opened)
	{
		if (!m_open)
		{
			if (m_tics != 0)
				m_tics--;

			if (m_tics == 0)
			{
				m_state = ECDoorState::Closing;
				SetHidden(false);

				m_sound[1]->Play();
			}
		}
	}
	else if (m_state == ECDoorState::Closing)
	{
		SetOffsetX(GetOffsetX() - m_speed * elapsedTime);

		if (GetOffsetX() < 0.0f)
		{
			m_state = ECDoorState::Closed;
			SetOffsetX(0.0f);

			m_mesh[0]->SetOffsetX(GetOffsetX());

			if (m_mesh[1] != nullptr)
				m_mesh[1]->SetOffsetX(GetOffsetX());

			return true;
		}

		m_mesh[0]->SetOffsetX(GetOffsetX());

		if (m_mesh[1] != nullptr)
			m_mesh[1]->SetOffsetX(GetOffsetX());
	}

	return false;
}

void CDoor::ToggleMesh()
{
	if ((GetId() & 0xA) == 0xA)
	{
		if (m_mesh[0] != nullptr)
			m_mesh[0]->SetHidden(!m_mesh[0]->GetHidden());

		if (m_mesh[1] != nullptr)
			m_mesh[1]->SetHidden(!m_mesh[1]->GetHidden());
	}
}