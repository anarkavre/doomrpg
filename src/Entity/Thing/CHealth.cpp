#include "CHealth.h"
#include "Player/CPlayer.h"

bool CHealth::Pickup()
{
	CPlayer &player = m_doomrpg->GetPlayer();
	const entity_t *entity = m_doomrpg->GetEntity(GetId());

	unsigned int gain = glm::min(player.GetHealth() + entity->parameter2, player.GetMaxHealth()) - player.GetHealth();

	if (gain != 0)
	{
		player.SetHealth(player.GetHealth() + gain);

		std::string message = m_doomrpg->GetBaseString(69);
		message.replace(message.find("%1"), 2, std::to_string(gain));
		m_doomrpg->GetStatus().AddMessage(message);

		return true;
	}
	else
	{
		std::string message = m_doomrpg->GetBaseString(39);
		m_doomrpg->GetStatus().AddMessage(message);

		return false;
	}
}