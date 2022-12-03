#include "CStatus.h"
#include "CDoomRPG.h"
#include "Entity/CEntity.h"
#include "Entity/Thing/CWeapon.h"
#include "Player/CPlayer.h"
#include "Resource/CFont.h"

CStatus::~CStatus()
{
	if (m_doomrpg != nullptr)
		m_doomrpg->GetGameLoop().UnregisterUpdateHandler(*this);
}

bool CStatus::Initialize(CDoomRPG *doomrpg)
{
	if (doomrpg != nullptr)
	{
		m_doomrpg = doomrpg;

		m_statusBar.Initialize(m_doomrpg, ECUIElementHorizontalAlignment::Left, ECUIElementVerticalAlignment::Top, { "k.bmp", 0, 0, 20, 20 }, ECUIElementProperties::RepeatTextureHorizontally | ECUIElementProperties::FillParentWidth);

		m_hudBars[0].Initialize(m_doomrpg, ECUIElementHorizontalAlignment::Left, ECUIElementVerticalAlignment::Bottom, { "k.bmp", 0, 0, 20, 20 }, ECUIElementProperties::RepeatTextureHorizontally | ECUIElementProperties::FillParentWidth);
		m_hudBars[1].Initialize(m_doomrpg, ECUIElementHorizontalAlignment::Left, ECUIElementVerticalAlignment::Bottom, { "bar_lg.bmp", 0, 0, 20, 28 }, ECUIElementProperties::RepeatTextureHorizontally | ECUIElementProperties::FillParentWidth);

		m_huds[0] = &m_hudBars[0].AddChild(ECUIElementHorizontalAlignment::Center, ECUIElementVerticalAlignment::Top, { "hud.png", 0, 0, 128, 20 });
		m_huds[1] = &m_hudBars[1].AddChild(ECUIElementHorizontalAlignment::Center, ECUIElementVerticalAlignment::Top, { "hud_lg.png", 0, 0, 176, 28 });

		m_faces[0] = &m_huds[0]->AddStatefulChild();
		m_faces[1] = &m_huds[1]->AddStatefulChild();

		for (int i = 0; i < 9; i++)
		{
			m_faces[0]->AddState({ 66, 0, 18, 20 }, { "l.bmp", 0, i * 18, 18, 20 });
			m_faces[1]->AddState({ 87, 0, 26, 28 }, { "larger HUD faces.bmp", 0, i * 28, 26, 28 });
		}

		m_healthIcons[0] = &m_huds[0]->AddChild({ 2, 4, 13, 13 }, { "m.bmp", 0, 0, 13, 13 });
		m_healthIcons[1] = &m_huds[1]->AddChild({ 2, 5, 18, 18 }, { "larger_HUD_icon_sheet.bmp", 0, 0, 18, 18 });

		m_armorIcons[0] = &m_huds[0]->AddChild({ 35, 4, 13, 13 }, { "m.bmp", 0, 13, 13, 13 });
		m_armorIcons[1] = &m_huds[1]->AddChild({ 46, 5, 18, 18 }, { "larger_HUD_icon_sheet.bmp", 0, 18, 18, 18 });

		m_weaponIcons[0] = &m_huds[0]->AddStatefulChild();
		m_weaponIcons[1] = &m_huds[1]->AddStatefulChild();

		for (int i = 0; i < 7; i++)
		{
			m_weaponIcons[0]->AddState({ 85, 4, 13, 13 }, { "m.bmp", 0, 26 + i * 13, 13, 13 });
			m_weaponIcons[1]->AddState({ 116, 5, 18, 18 }, { "larger_HUD_icon_sheet.bmp", 0, 36 + i * 18, 18, 18 });
		}

		m_directionIcons[0] = &m_huds[0]->AddChild({ 119, 2, 8, 5 }, { "o.bmp", 0, 0, 8, 5 });
		m_directionIcons[1] = &m_huds[1]->AddChild({ 164, 3, 8, 5 }, { "o.bmp", 0, 0, 8, 5 });

		m_hudBars[!m_currentHud].SetHidden(true);

		m_faces[0]->SetState(0);
		m_faces[1]->SetState(0);

		m_largeFont = m_doomrpg->GetResourceManager().AcquireResource<CFont>("larger_font.json");

		m_displayTics = static_cast<unsigned int>(1.0f * doomrpg->GetGameLoop().GetUpdateRate());

		m_initialized = true;
	}

	return m_initialized;
}

bool CStatus::Update(float elapsedTime)
{
	if (!m_initialized)
		return false;

	m_displayTicCount--;

	if (m_displayTicCount == 0)
	{
		m_displayTicCount = m_displayTics;

		if (m_messages.empty())
		{
			m_message = "";

			return true;
		}
		else
		{
			m_message = m_messages.front();
			m_messages.pop();
		}
	}

	return false;
}

void CStatus::Render()
{
	if (!m_initialized)
		return;

	std::shared_ptr<CFont> font = m_doomrpg->GetFont();

	CPlayer &player = m_doomrpg->GetPlayer();
	CEntity *target = static_cast<CEntity *>(player.GetTarget());

	if (!m_message.empty())
		m_statusBar.Print(font, 1, 5, m_message);
	else if (target != nullptr && target->GetName() != nullptr && glm::distance(target->GetPosition(), player.GetCamera().GetPosition()) <= 3.0f)
		m_statusBar.Print(font, 1, 5, target->GetName());

	if (!m_hidden)
	{
		CWeapon *weapon = player.GetCurrentWeapon();

		if (weapon->GetId() == ENTITY_AXE)
			m_currentWeaponIcon = 0;
		else if (weapon->GetId() == ENTITY_FIRE_EXT)
			m_currentWeaponIcon = 1;
		else if (weapon->GetId() == ENTITY_CHAINGUN || weapon->GetId() == ENTITY_PISTOL)
			m_currentWeaponIcon = 2;
		else if (weapon->GetId() == ENTITY_SHOTGUN || weapon->GetId() == ENTITY_SUPER_SHOTGN)
			m_currentWeaponIcon = 3;
		else if (weapon->GetId() == ENTITY_ROCKET_LNCHR)
			m_currentWeaponIcon = 4;
		else if (weapon->GetId() == ENTITY_PLASMA_GUN || weapon->GetId() == ENTITY_BFG)
			m_currentWeaponIcon = 5;
		else
			m_currentWeaponIcon = 6;

		if (m_currentWeaponIcon != m_lastWeaponIcon)
		{
			m_weaponIcons[0]->SetState(m_currentWeaponIcon);
			m_weaponIcons[1]->SetState(m_currentWeaponIcon);

			m_lastWeaponIcon = m_currentWeaponIcon;
		}

		float playerAngle = player.GetAngleY() + 90.0f;
		char *direction;

		if (playerAngle > 45.0f && playerAngle <= 135.0f)
			direction = "N";
		else if (playerAngle > 135.0f && playerAngle <= 225.0f)
			direction = "W";
		else if (playerAngle > 225.0f && playerAngle <= 315.0f)
			direction = "S";
		else
			direction = "E";

		if (m_currentHud == 0)
		{
			m_huds[0]->Print(font, 16, 5, (player.GetHealth() < 10 ? " " : "") + std::to_string(player.GetHealth()));
			m_huds[0]->Print(font, 48, 5, (player.GetArmor() < 10 ? " " : "") + std::to_string(player.GetArmor()));
			m_huds[0]->Print(font, 98, 5, (weapon->GetId() != ENTITY_AXE ? (weapon->GetAmmo() < 10 ? " " : "") + std::to_string(weapon->GetAmmo()) : "--"));
			m_huds[0]->Print(font, 118, 7, direction);
		}
		else
		{
			m_huds[1]->Print(m_largeFont, 21, 6, (player.GetHealth() < 10 ? " " : "") + std::to_string(player.GetHealth()));
			m_huds[1]->Print(m_largeFont, 64, 6, (player.GetArmor() < 10 ? " " : "") + std::to_string(player.GetArmor()));
			m_huds[1]->Print(m_largeFont, 134, 6, (weapon->GetId() != ENTITY_AXE ? (weapon->GetAmmo() < 10 ? " " : "") + std::to_string(weapon->GetAmmo()) : "--"));
			m_huds[1]->Print(m_largeFont, 161, 8, direction);
		}
	}
}

void CStatus::AddMessage(const std::string &message)
{
	if (!m_initialized)
		return;

	if (m_messages.empty())
	{
		m_doomrpg->GetGameLoop().RegisterUpdateHandler(*this);

		m_displayTicCount = 1;
	}

	m_messages.push(message);
}

void CStatus::ChangeHUD()
{
	if (!m_initialized)
		return;

	m_hudBars[m_currentHud].SetHidden(false);
	m_hudBars[!m_currentHud].SetHidden(true);
}

void CStatus::ToggleHUDVisibility()
{
	if (!m_hidden)
		ShowHUD();
	else
		HideHUD();
}
void CStatus::ShowHUD()
{
	m_huds[m_currentHud]->SetHidden(false);
}

void CStatus::HideHUD()
{
	m_huds[m_currentHud]->SetHidden(true);
}