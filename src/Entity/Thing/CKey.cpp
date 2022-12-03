#include "CKey.h"
#include "Player/CPlayer.h"

bool CKey::Pickup()
{
	CPlayer &player = m_doomrpg->GetPlayer();
	const entity_t *entity = m_doomrpg->GetEntity(GetId());

	player.SetKey(entity->parameter2, this);

	return true;
}