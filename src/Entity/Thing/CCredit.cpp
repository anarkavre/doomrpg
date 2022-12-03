#include "CCredit.h"
#include "Player/CPlayer.h"

bool CCredit::Pickup()
{
	CPlayer &player = m_doomrpg->GetPlayer();
	const entity_t *entity = m_doomrpg->GetEntity(GetId());

	unsigned int gain = glm::min(player.GetCredits() + entity->parameter2, player.GetMaxCredits()) - player.GetCredits();

	if (gain != 0)
	{
		player.SetCredits(player.GetCredits() + gain);

		return true;
	}
	else
	{
		std::string message = m_doomrpg->GetBaseString(38);
		m_doomrpg->GetStatus().AddMessage(message);

		return false;
	}
}