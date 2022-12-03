#include "CWeapon.h"
#include "Player/CPlayer.h"
#include "Resource/CMusic.h"
#include "Resource/CSound.h"

bool CWeapon::Update(float elapsedTime)
{
	if (m_doomrpg->GetPlayer().GetWeaponsMesh()->GetTextureAnimation().GetState() == "WEAPON_LOWERED")
	{
		m_firing = false;

		return true;
	}

	return false;
}

bool CWeapon::Pickup()
{
	CPlayer &player = m_doomrpg->GetPlayer();
	const entity_t *entity = m_doomrpg->GetEntity(GetId());

	CWeapon *weapon = player.GetWeapon(entity->parameter1);

	if (weapon != nullptr)
	{
		unsigned int gain = glm::min(weapon->GetAmmo() + entity->parameter2, 99u) - weapon->GetAmmo();

		if (gain != 0)
			weapon->SetAmmo(weapon->GetAmmo() + gain);
	}
	else
	{
		player.SetWeapon(entity->parameter1, this);
		player.SetCurrentWeapon(entity->parameter1);

		if (GetId() == ENTITY_FIRE_EXT)
			m_ammoType = 0;
		else if (GetId() == ENTITY_CHAINGUN || GetId() == ENTITY_PISTOL)
			m_ammoType = 1;
		else if (GetId() == ENTITY_SHOTGUN || GetId() == ENTITY_SUPER_SHOTGN)
			m_ammoType = 2;
		else if (GetId() == ENTITY_ROCKET_LNCHR)
			m_ammoType = 3;
		else if (GetId() == ENTITY_PLASMA_GUN || GetId() == ENTITY_BFG)
			m_ammoType = 4;

		if (GetId() == ENTITY_PISTOL)
			SetAmmo(8);
		else if (GetId() != ENTITY_AXE)
		{
			unsigned int gain = glm::min(GetAmmo() + entity->parameter2, 99u) - GetAmmo();

			if (gain != 0)
				SetAmmo(GetAmmo() + gain);
		}

		if (GetId() == ENTITY_AXE)
			m_ammoUsed = 0;
		else if (GetId() == ENTITY_FIRE_EXT || GetId() == ENTITY_SHOTGUN || GetId() == ENTITY_ROCKET_LNCHR || GetId() == ENTITY_PISTOL)
			m_ammoUsed = 1;
		else if (GetId() == ENTITY_SUPER_SHOTGN)
			m_ammoUsed = 2;
		else if (GetId() == ENTITY_CHAINGUN || GetId() == ENTITY_PLASMA_GUN)
			m_ammoUsed = 3;
		else if (GetId() == ENTITY_BFG)
			m_ammoUsed = 15;

		std::string message;

		if (GetId() == ENTITY_AXE)
			message = m_doomrpg->GetBaseString(42);
		else if (GetId() == ENTITY_FIRE_EXT)
			message = m_doomrpg->GetBaseString(45);
		else if (GetId() == ENTITY_SHOTGUN)
			message = m_doomrpg->GetBaseString(48);
		else if (GetId() == ENTITY_SUPER_SHOTGN)
			message = m_doomrpg->GetBaseString(49);
		else if (GetId() == ENTITY_CHAINGUN)
			message = m_doomrpg->GetBaseString(44);
		else if (GetId() == ENTITY_ROCKET_LNCHR)
			message = m_doomrpg->GetBaseString(47);
		else if (GetId() == ENTITY_PLASMA_GUN)
			message = m_doomrpg->GetBaseString(46);
		else if (GetId() == ENTITY_BFG)
			message = m_doomrpg->GetBaseString(43);

		m_doomrpg->GetDialog().SetMessage(message);
		m_doomrpg->GetDialog().Show();
		m_doomrpg->GetDialog().Lock();
	}

	return true;
}

void CWeapon::Fire()
{
	if (m_firing)
		return;

	if (m_ammoUsed != 0)
	{
		if (GetAmmo() < m_ammoUsed)
		{
			std::string message = m_doomrpg->GetBaseString(25);
			m_doomrpg->GetStatus().AddMessage(message);

			return;
		}

		SetAmmo(GetAmmo() - m_ammoUsed);
	}

	m_doomrpg->GetGameLoop().RegisterUpdateHandler(*this);

	m_doomrpg->GetPlayer().GetWeaponsMesh()->GetTextureAnimation().SetState("WEAPON_FIRING");

	m_firing = true;
}

unsigned int CWeapon::GetAmmo() const
{
	return m_doomrpg->GetPlayer().GetAmmo(m_ammoType);
}

void CWeapon::SetAmmo(unsigned int ammo)
{
	m_doomrpg->GetPlayer().SetAmmo(m_ammoType, ammo);
}