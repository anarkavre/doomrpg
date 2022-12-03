#include "CBerserker.h"
#include "Player/CPlayer.h"

bool CBerserker::Pickup()
{
	CPlayer &player = m_doomrpg->GetPlayer();
	const entity_t *entity = m_doomrpg->GetEntity(GetId());

	if (player.GetBerserkers() < 99)
	{
		player.SetBerserkers(player.GetBerserkers() + 1);

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