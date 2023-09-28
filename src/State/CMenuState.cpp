#include "glm/gtx/transform.hpp"

#include "CDoomRPG.h"
#include "CMenuState.h"
#include "Resource/CMusic.h"

bool CMenuState::Initialize(CGame* game)
{
	if (CGameState::Initialize(game))
	{
		m_doomrpg = static_cast<CDoomRPG *>(m_game);

		static_cast<CTurnBasedCamera *>(&m_doomrpg->GetPlayer().GetCamera())->Lock();
		m_doomrpg->GetPlayer().GetWeaponsMesh()->SetHidden(true);

		m_doomrpg->LoadMap("menu.bsp");

		m_music = m_doomrpg->GetResourceManager().AcquireResource<CMusic>("t.mid");
		m_music->SetRepeat(true);
		m_music->Play();

		m_initialized = true;
	}

	return m_initialized;
}

void CMenuState::Update()
{
	float angle = m_doomrpg->GetPlayer().GetCamera().GetAngleY();
	angle += 18.0f * m_doomrpg->GetTimer().GetElapsedTime();
	m_doomrpg->GetPlayer().GetCamera().SetAngleY(angle);
}