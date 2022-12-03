#include "CAmmo.h"
#include "Player/CPlayer.h"

bool CAmmo::Pickup()
{
	CPlayer &player = m_doomrpg->GetPlayer();
	const entity_t *entity = m_doomrpg->GetEntity(GetId());

	unsigned int gain = glm::min(player.GetAmmo(entity->parameter1) + entity->parameter2, 99u) - player.GetAmmo(entity->parameter1);

	if (gain != 0)
	{
		player.SetAmmo(entity->parameter1, player.GetAmmo(entity->parameter1) + gain);

		return true;
	}
	else
	{
		std::string message = m_doomrpg->GetBaseString(36);
		m_doomrpg->GetStatus().AddMessage(message);

		return false;
	}
}