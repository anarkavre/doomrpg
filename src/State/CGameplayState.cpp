#include "glm/gtx/intersect.hpp"

#include "CDoomRPG.h"
#include "CGameplayState.h"
#include "Resource/CMap.h"
#include "Resource/CSound.h"
#include "doomrpg_sounds.h"

bool CGameplayState::Initialize(CGame *game)
{
	if (CGameState::Initialize(game))
	{
		m_doomrpg = static_cast<CDoomRPG *>(m_game);

		CPlayer &player = m_doomrpg->GetPlayer();
		player.SetHealth(30);
		player.SetMaxHealth(30);
		player.SetArmor(0);
		player.SetMaxArmor(30);
		player.SetCredits(0);
		player.SetMaxCredits(UINT_MAX);

		m_doomrpg->LoadMap("intro.bsp");

		m_doomrpg->GetStatus().Initialize(m_doomrpg);
		m_doomrpg->GetDialog().Initialize(m_doomrpg);

		m_noUse = m_doomrpg->GetResourceManager().AcquireResource<CSound>(std::string(SOUND_NO_USE) + ".wav");

		m_initialized = true;
	}

	return m_initialized;
}

void CGameplayState::Update()
{
	CPlayer &player = m_doomrpg->GetPlayer();
	CCamera &camera = player.GetCamera();
	std::shared_ptr<CMap> map = m_doomrpg->GetMap();

	m_nodeIndices.clear();

	CScenePartitionTree &scenePartitionTree = m_doomrpg->GetSceneSystem().GetScenePartitionTree();

	scenePartitionTree.FindLeafNodes(camera, m_nodeIndices);

	float minDistance = FLT_MAX;
	CObject *target = nullptr;

	for (unsigned int i = 0; i < m_nodeIndices.size(); i++)
	{
		const std::vector<CSceneEntity> &sceneEntities = *scenePartitionTree.GetSceneEntities(m_nodeIndices[i]);

		for (unsigned int i = 0; i < sceneEntities.size(); i++)
		{
			std::shared_ptr<CObject> object = sceneEntities[i].GetObject();

			if (static_cast<CMesh *>(object.get())->GetID() < UINT16_MAX)
			{
				std::vector<unsigned int> entityIndices;

				map->GetEntities(static_cast<unsigned int>(object->GetX()), static_cast<unsigned int>(object->GetZ()), entityIndices);

				for (unsigned int i = 0; i < entityIndices.size(); i++)
				{
					CEntity *entity = const_cast<CEntity *>(map->GetEntity(entityIndices[i]));

					if (!entity->IsHidden())
					{
						if (entity->GetType() == ENTITY_TYPE_DOOR)
						{
							float distance;

							if (glm::intersectRaySphere(camera.GetPosition(), camera.GetForwardVector(), entity->GetPosition(), 0.25f, distance))
							{
								if (distance <= 8.0f && distance < minDistance)
								{
									minDistance = distance;
									target = entity;
								}
							}
						}
						else if (entity->GetId() != ENTITY_LIGHT_1 && entity->GetId() != ENTITY_LIGHT_2)
						{
							float distance;

							if (glm::intersectRaySphere(camera.GetPosition(), camera.GetForwardVector(), entity->GetPosition(), 0.25f, distance))
							{
								if (distance <= 7.5f && distance < minDistance)
								{
									minDistance = distance;
									target = entity;
								}
							}
						}
					}
				}
			}
		}
	}

	player.SetTarget(target);

	m_doomrpg->GetStatus().Render();
	m_doomrpg->GetDialog().Render();
}

bool CGameplayState::RunActionEvent(Event *event)
{
	bool isLastCommand = false;

	if (event != nullptr && event->onActionCommands.size() != 0)
	{
		if (!m_doomrpg->GetDialog().IsShown())
			event->halted = false;

		if (!event->halted)
		{
			if (m_doomrpg->GetDialog().IsCanceled() && event->currentCommand != 0)
				event->currentCommand--;

			unsigned int variable = event->variable;

			const std::vector<unsigned int> &commands = event->onActionCommands[variable];

			isLastCommand = (commands.empty() || commands[event->currentCommand] == event->commandCount - 1);

			while (!event->halted && event->currentCommand < commands.size())
			{
				m_doomrpg->GetMap()->ExecuteCommand(event, commands[event->currentCommand]);

				event->currentCommand++;
			}

			if (event->currentCommand == commands.size())
				event->currentCommand = 0;
		}
	}
	else
	{
		std::string message = m_doomrpg->GetBaseString(66);
		m_doomrpg->GetStatus().AddMessage(message);

		m_noUse->Play();
	}

	return isLastCommand;
}

bool CGameplayState::RunEnterEvent(Event *event, unsigned int direction)
{
	bool isLastCommand = false;

	if (event != nullptr && event->onEnterCommands.size() != 0)
	{
		if (!m_doomrpg->GetDialog().IsShown())
			event->halted = false;

		if (!event->halted)
		{
			unsigned int variable = event->variable;

			const std::vector<unsigned int> &commands = event->onEnterCommands[variable][direction];

			isLastCommand = (commands.empty() || commands[event->currentCommand] == event->commandCount - 1);

			while (!event->halted && event->currentCommand < commands.size())
			{
				m_doomrpg->GetMap()->ExecuteCommand(event, commands[event->currentCommand]);

				event->currentCommand++;
			}

			if (event->currentCommand == commands.size())
				event->currentCommand = 0;
		}
	}

	return isLastCommand;
}

bool CGameplayState::RunLeaveEvent(Event *event, unsigned int direction)
{
	bool isLastCommand = false;

	if (event != nullptr && event->onLeaveCommands.size() != 0)
	{
		if (!m_doomrpg->GetDialog().IsShown())
			event->halted = false;

		if (!event->halted)
		{
			unsigned int variable = event->variable;

			const std::vector<unsigned int> &commands = event->onLeaveCommands[variable][direction];

			isLastCommand = (commands.empty() || commands[event->currentCommand] == event->commandCount - 1);

			while (!event->halted && event->currentCommand < commands.size())
			{
				m_doomrpg->GetMap()->ExecuteCommand(event, commands[event->currentCommand]);

				event->currentCommand++;
			}

			if (event->currentCommand == commands.size())
				event->currentCommand = 0;
		}
	}

	return isLastCommand;
}

bool CGameplayState::RunLookEvent(Event *event, unsigned int direction)
{
	bool isLastCommand = false;

	if (event != nullptr && event->onLookCommands[direction].size() != 0)
	{
		if (!m_doomrpg->GetDialog().IsShown())
			event->halted = false;

		if (!event->halted)
		{
			const std::vector<unsigned int> &commands = event->onLookCommands[direction];

			isLastCommand = (commands.empty() || commands[event->currentCommand] == event->commandCount - 1);

			while (!event->halted && event->currentCommand < commands.size())
			{
				m_doomrpg->GetMap()->ExecuteCommand(event, commands[event->currentCommand]);

				event->currentCommand++;
			}

			if (event->currentCommand == commands.size())
				event->currentCommand = 0;
		}
	}

	return isLastCommand;
}