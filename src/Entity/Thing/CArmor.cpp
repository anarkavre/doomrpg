#include "CArmor.h"
#include "Player/CPlayer.h"

bool CArmor::Pickup()
{
	CPlayer &player = m_doomrpg->GetPlayer();
	const entity_t *entity = m_doomrpg->GetEntity(GetId());

	unsigned int gain = glm::min(player.GetArmor() + entity->parameter2, player.GetMaxArmor()) - player.GetArmor();

	if (gain != 0)
	{
		player.SetArmor(player.GetArmor() + gain);

		std::string message = m_doomrpg->GetBaseString(68);
		message.replace(message.find("%1"), 2, std::to_string(gain));
		m_doomrpg->GetStatus().AddMessage(message);

		return true;
	}
	else
	{
		std::string message = m_doomrpg->GetBaseString(37);
		m_doomrpg->GetStatus().AddMessage(message);

		return false;
	}
}