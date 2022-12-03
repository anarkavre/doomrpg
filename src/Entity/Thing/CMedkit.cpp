#include "CMedkit.h"
#include "Player/CPlayer.h"

bool CMedkit::Pickup()
{
	CPlayer &player = m_doomrpg->GetPlayer();
	const entity_t *entity = m_doomrpg->GetEntity(GetId());

	if (entity->parameter1 == ENTITY_CONSUMABLE_SM_MEDKIT)
	{
		if (player.GetSmallMedkits() < 99)
		{
			player.SetSmallMedkits(player.GetSmallMedkits() + 1);

			return true;
		}
		else
		{
			std::string message = m_doomrpg->GetBaseString(41);
			message.replace(message.find("%1"), 2, m_doomrpg->GetEntityString(GetId()));
			m_doomrpg->GetStatus().AddMessage(message);

			return false;
		}
	}
	else
	{
		if (player.GetLargeMedkits() < 99)
		{
			player.SetLargeMedkits(player.GetLargeMedkits() + 1);

			return true;
		}
		else
		{
			std::string message = m_doomrpg->GetBaseString(41);
			message.replace(message.find("%1"), 2, m_doomrpg->GetEntityString(GetId()));
			m_doomrpg->GetStatus().AddMessage(message);

			return false;
		}
	}
}