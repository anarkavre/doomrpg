#include <vector>

#include "CCollisionHandler.h"
#include "Entity/CEntity.h"
#include "Object/CObject.h"
#include "Resource/CMap.h"

void CCollisionHandler::HandleCollision(CObject *object)
{
	if (m_map == nullptr)
		return;

	glm::vec3 directionVector = object->GetPosition() - object->GetOldPosition();

	if (directionVector != glm::vec3(0.0f))
		directionVector = glm::normalize(directionVector);

	unsigned int blockX = static_cast<unsigned int>(object->GetOldX() + directionVector.x);
	unsigned int blockY = static_cast<unsigned int>(object->GetOldZ() + directionVector.z);
	unsigned int oldBlockX = static_cast<unsigned int>(object->GetOldX());
	unsigned int oldBlockY = static_cast<unsigned int>(object->GetOldZ());

	if (m_map->GetBlock(blockX, blockY) & 0x1)
		object->SetPosition(object->GetOldPosition());
	else
	{
		std::vector<unsigned int> entities;

		m_map->GetEntities(blockX, blockY, entities);

		for (unsigned int i = 0; i < entities.size(); i++)
		{
			const CEntity *entity = m_map->GetEntity(entities[i]);

			if (!entity->IsHidden())
			{
				if (entity->GetType() == ENTITY_TYPE_DOOR || entity->GetType() == ENTITY_TYPE_ENEMY || entity->GetType() == ENTITY_TYPE_HUMAN || entity->GetType() == ENTITY_TYPE_USABLE || entity->GetType() == ENTITY_TYPE_DESTRUCTIBLE || entity->GetType() == ENTITY_TYPE_STATIC || entity->GetType() == 18)
					object->SetPosition(object->GetOldPosition());
				else if (entity->GetType() == 17)
				{
					std::vector<unsigned int> entities;

					m_map->GetEntities(oldBlockX, oldBlockY, entities);

					for (unsigned int i = 0; i < entities.size(); i++)
					{
						const CEntity *entity = m_map->GetEntity(entities[i]);

						if (entity->GetType() == 17)
							object->SetPosition(object->GetOldPosition());
					}
				}
			}
		}
	}
}