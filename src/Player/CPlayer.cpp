#include "CDoomRPG.h"
#include "CPlayer.h"
#include "Camera/CTurnBasedCamera.h"
#include "Entity/CDoor.h"
#include "Entity/Thing/CWeapon.h"
#include "Geometry/CAnimatedQuadList.h"
#include "Resource/CMap.h"
#include "Resource/CSound.h"
#include "Window/CWindow.h"
#include "doomrpg_sounds.h"

CPlayer::~CPlayer()
{
	if (m_pistol != nullptr)
		delete m_pistol;
}

bool CPlayer::Initialize(CDoomRPG *doomrpg)
{
	if (!m_initialized && doomrpg != nullptr)
	{
		m_doomrpg = doomrpg;

		m_doomrpg->GetGameLoop().RegisterUpdateHandler(*this);

		m_camera.Initialize(m_doomrpg, 90.0f, float(m_doomrpg->GetWindow().GetWidth()) / float(m_doomrpg->GetWindow().GetHeight()), 0.1f, 10.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 90.0f);
		m_doomrpg->GetCameraSystem().SetViewCamera(m_camera);

		m_action = &m_doomrpg->GetInputSystem().RegisterAction("action");
		m_doomrpg->GetInputSystem().BindAction("action", SDLK_e);

		m_weaponAction[0] = &m_doomrpg->GetInputSystem().RegisterAction("weapon0Action");
		m_weaponAction[1] = &m_doomrpg->GetInputSystem().RegisterAction("weapon1Action");
		m_weaponAction[2] = &m_doomrpg->GetInputSystem().RegisterAction("weapon2Action");
		m_weaponAction[3] = &m_doomrpg->GetInputSystem().RegisterAction("weapon3Action");
		m_weaponAction[4] = &m_doomrpg->GetInputSystem().RegisterAction("weapon4Action");
		m_weaponAction[5] = &m_doomrpg->GetInputSystem().RegisterAction("weapon5Action");
		m_weaponAction[6] = &m_doomrpg->GetInputSystem().RegisterAction("weapon6Action");
		m_weaponAction[7] = &m_doomrpg->GetInputSystem().RegisterAction("weapon7Action");
		m_weaponAction[8] = &m_doomrpg->GetInputSystem().RegisterAction("weapon8Action");
		m_doomrpg->GetInputSystem().BindAction("weapon0Action", SDLK_1);
		m_doomrpg->GetInputSystem().BindAction("weapon1Action", SDLK_2);
		m_doomrpg->GetInputSystem().BindAction("weapon2Action", SDLK_3);
		m_doomrpg->GetInputSystem().BindAction("weapon3Action", SDLK_4);
		m_doomrpg->GetInputSystem().BindAction("weapon4Action", SDLK_5);
		m_doomrpg->GetInputSystem().BindAction("weapon5Action", SDLK_6);
		m_doomrpg->GetInputSystem().BindAction("weapon6Action", SDLK_7);
		m_doomrpg->GetInputSystem().BindAction("weapon7Action", SDLK_8);
		m_doomrpg->GetInputSystem().BindAction("weapon8Action", SDLK_9);

		std::shared_ptr<CAnimatedQuadList> animatedQuadList = m_doomrpg->GetGeometryManager().AcquireGeometry<CAnimatedQuadList>(0xF0, "weapons");

		for (uint16_t id = 0xF0; id <= 0xF8; id++)
		{
			unsigned int textureIndex = m_doomrpg->GetMappings()->thingMappings[id];

			animatedQuadList->Add(textureIndex);
		}

		std::shared_ptr<CMaterial> material = m_doomrpg->GetMaterialManager().AcquireMaterial(7, "sprite");
		material->SetTexture(m_doomrpg->GetSprites());

		m_weaponsMesh = m_doomrpg->GetMeshManager().AcquireMesh<CAnimatedMeshStateful>(0xF0, "weapons");
		m_weaponsMesh->Initialize(std::move(animatedQuadList), std::move(material));
		m_weaponsMesh->RegisterCollisionHandler(m_collisionHandler);

		for (uint16_t id = 0xF0; id <= 0xF8; id++)
		{
			std::string animation;

			if (id == 0xF0)
				animation = "axe";
			else if (id == 0xF1)
				animation = "fireExtinguisher";
			else if (id == 0xF2)
				animation = "pistol";
			else if (id == 0xF3)
				animation = "shotgun";
			else if (id == 0xF4)
				animation = "chaingun";
			else if (id == 0xF5)
				animation = "superShotgun";
			else if (id == 0xF6)
				animation = "plasmaGun";
			else if (id == 0xF7)
				animation = "rocketLauncher";
			else if (id == 0xF8)
				animation = "bfg";

			m_weaponsMesh->AddState(animation);
		}

		float near = m_camera.GetNearPlaneDistance() + 0.0001f;

		m_weaponsMesh->SetOffsetZ(-near);
		m_weaponsMesh->SetScaleX(near);
		m_weaponsMesh->SetScaleY(near);

		m_camera.SetChild(m_weaponsMesh);

		thing_t pistol;
		pistol.id = ENTITY_PISTOL;

		m_pistol = new CWeapon(m_doomrpg, &pistol);
		m_pistol->Pickup();

		m_itemPickup = m_doomrpg->GetResourceManager().AcquireResource<CSound>(std::string(SOUND_ITEM_PICKUP) + ".wav");
		m_weaponPickup = m_doomrpg->GetResourceManager().AcquireResource<CSound>(std::string(SOUND_WEAPON_PICKUP) + ".wav");
		m_powerupPickup = m_doomrpg->GetResourceManager().AcquireResource<CSound>(std::string(SOUND_POWERUP_PICKUP) + ".wav");
		m_noUse = m_doomrpg->GetResourceManager().AcquireResource<CSound>(std::string(SOUND_NO_USE) + ".wav");

		m_initialized = true;
	}

	return m_initialized;
}

bool CPlayer::Update(float elapsedTime)
{
	if (!m_camera.IsMoving() && !m_camera.IsTurning() && !m_camera.IsLocked())
	{
		unsigned int blockX = static_cast<unsigned int>(m_camera.GetX());
		unsigned int blockY = static_cast<unsigned int>(m_camera.GetZ());
		unsigned int oldBlockX = static_cast<unsigned int>(m_camera.GetOldX());
		unsigned int oldBlockY = static_cast<unsigned int>(m_camera.GetOldZ());

		if (blockX != oldBlockX || blockY != oldBlockY)
		{
			std::vector<unsigned int> things;

			m_doomrpg->GetMap()->GetThings(blockX, blockY, things);

			for (unsigned int i = 0; i < things.size(); i++)
			{
				const CThing *thing = m_doomrpg->GetMap()->GetThing(things[i]);

				if (((thing->GetType() > ENTITY_TYPE_HUMAN && thing->GetType() < ENTITY_TYPE_USABLE) || thing->GetType() == ENTITY_TYPE_AMMO_MULTIPLE) && !thing->IsHidden())
				{
					if (const_cast<CThing *>(thing)->Pickup())
					{
						const_cast<CThing *>(thing)->SetHidden(true);

						std::string message = m_doomrpg->GetBaseString(40);
						message.replace(message.find("%1"), 2, thing->GetName());
						m_doomrpg->GetStatus().AddMessage(message);

						if (thing->GetId() >= ENTITY_SHOTGUN && thing->GetId() <= ENTITY_PISTOL)
							m_weaponPickup->Play();
						else if (thing->GetId() >= ENTITY_SOUL_SPHERE && thing->GetId() <= ENTITY_DOG_COLLAR)
							m_powerupPickup->Play();
						else
							m_itemPickup->Play();
					}
					else
					{
						m_camera.SetOldX(m_camera.GetX());
						m_camera.SetOldZ(m_camera.GetZ());
					}
				}
			}

			Event *event = m_doomrpg->GetMap()->GetEvent(blockX, blockY);
			unsigned int direction;

			if (blockY > oldBlockY)
				direction = 0;
			else if (blockX < oldBlockX)
				direction = 1;
			else if (blockY < oldBlockY)
				direction = 2;
			else if (blockX > oldBlockX)
				direction = 3;

			bool isLastCommand = m_doomrpg->GetGameplayState().RunEnterEvent(event, direction);

			if (isLastCommand)
			{
				m_camera.SetOldX(m_camera.GetX());
				m_camera.SetOldZ(m_camera.GetZ());

				m_action->SetPressed(false);
			}

			float playerAngle = m_camera.GetAngleY() + 90.0f;

			if (playerAngle > 45.0f && playerAngle <= 135.0f)
				direction = 0;
			else if (playerAngle > 135.0f && playerAngle <= 225.0f)
				direction = 3;
			else if (playerAngle > 225.0f && playerAngle <= 315.0f)
				direction = 2;
			else
				direction = 1;

			if (!m_doomrpg->GetDialog().IsShown())
				m_doomrpg->GetGameplayState().RunLookEvent(event, direction);

			event = m_doomrpg->GetMap()->GetEvent(oldBlockX, oldBlockY);

			if (blockY < oldBlockY)
				direction = 0;
			else if (blockX > oldBlockX)
				direction = 1;
			else if (blockY > oldBlockY)
				direction = 2;
			else if (blockX < oldBlockX)
				direction = 3;

			isLastCommand = m_doomrpg->GetGameplayState().RunLeaveEvent(event, direction);

			if (isLastCommand)
			{
				m_camera.SetOldX(m_camera.GetX());
				m_camera.SetOldZ(m_camera.GetZ());

				m_action->SetPressed(false);
			}
		}

		if (!IsAttacking())
		{
			if (m_action->IsPressed())
			{
				static bool isLastCommand = false;

				CEntity *target = static_cast<CEntity *>(GetTarget());

				if (target != nullptr && (target->GetType() == ENTITY_TYPE_ENEMY || target->GetType() == ENTITY_TYPE_FIRE || target->GetType() == ENTITY_TYPE_DESTRUCTIBLE))
				{
					Attack();

					if (!IsAttacking())
					{
						std::string message = m_doomrpg->GetBaseString(66);
						m_doomrpg->GetStatus().AddMessage(message);

						m_noUse->Play();
					}
				}
				else if (target != nullptr && target->GetType() == ENTITY_TYPE_DOOR && static_cast<CDoor *>(target)->GetState() == ECDoorState::Locked)
				{
					std::string message = m_doomrpg->GetBaseString(66);
					m_doomrpg->GetStatus().AddMessage(message);

					m_noUse->Play();
				}
				else if (m_doomrpg->GetDialog().IsCanceled() || !isLastCommand)
				{
					float playerAngle = m_camera.GetAngleY() + 90.0f;

					if (playerAngle > 45.0f && playerAngle <= 135.0f)
						blockY--;
					else if (playerAngle > 135.0f && playerAngle <= 225.0f)
						blockX--;
					else if (playerAngle > 225.0f && playerAngle <= 315.0f)
						blockY++;
					else
						blockX++;

					Event* event = m_doomrpg->GetMap()->GetEvent(blockX, blockY);

					isLastCommand = m_doomrpg->GetGameplayState().RunActionEvent(event);

					if (!m_doomrpg->GetDialog().IsShown() && isLastCommand)
						isLastCommand = false;
				}
				else
					isLastCommand = false;

				m_action->SetPressed(false);
			}

			if (m_weaponAction[0]->IsPressed())
			{
				if (GetWeapon(0) != nullptr)
					SetCurrentWeapon(0);

				m_weaponAction[0]->SetPressed(false);
			}
			else if (m_weaponAction[1]->IsPressed())
			{
				if (GetWeapon(1) != nullptr)
					SetCurrentWeapon(1);

				m_weaponAction[1]->SetPressed(false);
			}
			else if (m_weaponAction[2]->IsPressed())
			{
				if (GetWeapon(2) != nullptr)
					SetCurrentWeapon(2);

				m_weaponAction[2]->SetPressed(false);
			}
			else if (m_weaponAction[3]->IsPressed())
			{
				if (GetWeapon(3) != nullptr)
					SetCurrentWeapon(3);

				m_weaponAction[3]->SetPressed(false);
			}
			else if (m_weaponAction[4]->IsPressed())
			{
				if (GetWeapon(4) != nullptr)
					SetCurrentWeapon(4);

				m_weaponAction[4]->SetPressed(false);
			}
			else if (m_weaponAction[5]->IsPressed())
			{
				if (GetWeapon(5) != nullptr)
					SetCurrentWeapon(5);

				m_weaponAction[5]->SetPressed(false);
			}
			else if (m_weaponAction[6]->IsPressed())
			{
				if (GetWeapon(6) != nullptr)
					SetCurrentWeapon(6);

				m_weaponAction[6]->SetPressed(false);
			}
			else if (m_weaponAction[7]->IsPressed())
			{
				if (GetWeapon(7) != nullptr)
					SetCurrentWeapon(7);

				m_weaponAction[7]->SetPressed(false);
			}
			else if (m_weaponAction[8]->IsPressed())
			{
				if (GetWeapon(8) != nullptr)
					SetCurrentWeapon(8);

				m_weaponAction[8]->SetPressed(false);
			}
		}
	}

	if (IsAttacking() && !GetCurrentWeapon()->IsFiring())
	{
		CEntity *target = static_cast<CEntity *>(GetTarget());

		float distance = glm::distance(target->GetPosition(), m_camera.GetPosition());

		if (target->GetType() == ENTITY_TYPE_FIRE)
		{
			if (distance <= 1.0f)
			{
				static_cast<CThing *>(target)->SetHidden(true);

				std::string message = m_doomrpg->GetBaseString(33);
				m_doomrpg->GetStatus().AddMessage(message);
			}
			else
			{
				std::string message = m_doomrpg->GetBaseString(24);
				m_doomrpg->GetStatus().AddMessage(message);
			}
		}
		else if (target->GetType() == ENTITY_TYPE_DESTRUCTIBLE)
		{
			if (GetCurrentWeapon()->GetId() == ENTITY_FIRE_EXT)
			{
				std::string message = m_doomrpg->GetBaseString(24);
				m_doomrpg->GetStatus().AddMessage(message);
			}
			else
			{
				const entity_t *entity = m_doomrpg->GetEntity(target->GetId());

				if (entity->parameter1 == ENTITY_DESTRUCTIBLE_BARREL)
					static_cast<CThing *>(target)->SetHidden(true);
				else if (entity->parameter1 == ENTITY_DESTRUCTIBLE_CRATE)
					static_cast<CThing *>(target)->SetHidden(true);
				else if (entity->parameter1 == ENTITY_DESTRUCTIBLE_DOOR_WALL)
				{
					if (distance <= 1.0f)
					{
						unsigned int blockX = static_cast<unsigned int>(m_camera.GetX());
						unsigned int blockY = static_cast<unsigned int>(m_camera.GetZ());

						float playerAngle = m_camera.GetAngleY() + 90.0f;

						if (playerAngle > 45.0f && playerAngle <= 135.0f)
							blockY--;
						else if (playerAngle > 135.0f && playerAngle <= 225.0f)
							blockX--;
						else if (playerAngle > 225.0f && playerAngle <= 315.0f)
							blockY++;
						else
							blockX++;

						Event *event = m_doomrpg->GetMap()->GetEvent(blockX, blockY);
						m_doomrpg->GetGameplayState().RunActionEvent(event);
					}
					else
					{
						std::string message = m_doomrpg->GetBaseString(23);
						m_doomrpg->GetStatus().AddMessage(message);
					}
				}
			}
		}

		m_attacking = false;
	}

	return false;
}

void CPlayer::Attack()
{
	CEntity *target = static_cast<CEntity *>(GetTarget());

	if (target->GetType() == ENTITY_TYPE_FIRE && GetCurrentWeapon()->GetId() == ENTITY_FIRE_EXT)
	{
		float distance = glm::distance(target->GetPosition(), m_camera.GetPosition());

		if (distance <= 8.0f)
		{
			GetCurrentWeapon()->Fire();

			m_attacking = true;
		}
	}
	else if (target->GetType() == ENTITY_TYPE_DESTRUCTIBLE)
	{
		const entity_t *entity = m_doomrpg->GetEntity(target->GetId());

		if (entity->parameter2 == 0xFFF || entity->parameter2 == GetCurrentWeapon()->GetId())
		{
			GetCurrentWeapon()->Fire();

			m_attacking = true;
		}
	}
}