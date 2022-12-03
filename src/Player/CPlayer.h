#ifndef __CPLAYER_H__
#define __CPLAYER_H__

#include "Camera/CTurnBasedCamera.h"
#include "Collision/CCollisionHandler.h"
#include "Mesh/CAnimatedMeshStateful.h"
#include "Object/CObject.h"

class CDoomRPG;
class CInputAction;
class CKey;
class CSound;
class CWeapon;

class CPlayer : public CObject
{
public:
	CPlayer(CDoomRPG *doomrpg = nullptr) : m_initialized(false), m_attacking(false), m_health(0), m_maxHealth(0), m_armor(0), m_maxArmor(0), m_credits(0), m_maxCredits(0), m_ammo{ 0 }, m_smallMedkits(0), m_largeMedkits(0), m_soulSpheres(0), m_berserkers(0), m_dogCollars(0), m_keys{ nullptr }, m_weapons{ nullptr } { Initialize(doomrpg); }
	~CPlayer();

	bool Initialize(CDoomRPG *doomrpg);
	bool Update(float elaspedTime);
	void Attack();

	CCamera &GetCamera() { return m_camera; }
	unsigned int GetHealth() const { return m_health; }
	void SetHealth(unsigned int health) { m_health = health; }
	unsigned int GetMaxHealth() const { return m_maxHealth; }
	void SetMaxHealth(unsigned int maxHealth) { m_maxHealth = maxHealth; }
	unsigned int GetArmor() const { return m_armor; }
	void SetArmor(unsigned int armor) { m_armor = armor; }
	unsigned int GetMaxArmor() const { return m_maxArmor; }
	void SetMaxArmor(unsigned int maxArmor) { m_maxArmor = maxArmor; }
	unsigned int GetCredits() const { return m_credits; }
	void SetCredits(unsigned int credits) { m_credits = credits; }
	unsigned int GetMaxCredits() const { return m_maxCredits; }
	void SetMaxCredits(unsigned int maxCredits) { m_maxCredits = maxCredits; }
	unsigned int GetAmmo(unsigned int type) const { return m_ammo[type]; }
	void SetAmmo(unsigned int type, unsigned int ammo) { m_ammo[type] = ammo; }
	unsigned int GetSmallMedkits() const { return m_smallMedkits; }
	void SetSmallMedkits(unsigned int smallMedkits) { m_smallMedkits = smallMedkits; }
	unsigned int GetLargeMedkits() const { return m_largeMedkits; }
	void SetLargeMedkits(unsigned int largeMedkits) { m_largeMedkits = largeMedkits; }
	unsigned int GetSoulSpheres() const { return m_soulSpheres; }
	void SetSoulSpheres(unsigned int soulSpheres) { m_soulSpheres = soulSpheres; }
	unsigned int GetBerserkers() const { return m_berserkers; }
	void SetBerserkers(unsigned int berserkers) { m_berserkers = berserkers; }
	unsigned int GetDogCollars() const { return m_dogCollars; }
	void SetDogCollars(unsigned int dogCollars) { m_dogCollars = dogCollars; }
	CKey *GetKey(unsigned int position) const { return m_keys[position]; }
	void SetKey(unsigned int position, CKey *key) { m_keys[position] = key; }
	CWeapon *GetWeapon(unsigned int position) const { return m_weapons[position]; }
	void SetWeapon(unsigned int position, CWeapon *weapon) { m_weapons[position] = weapon; }
	CWeapon *GetCurrentWeapon() const { return m_weapons[m_currentWeapon]; }
	void SetCurrentWeapon(unsigned int position) { m_currentWeapon = position; m_weaponsMesh->SetState(m_currentWeapon); }
	std::shared_ptr<CAnimatedMeshStateful> GetWeaponsMesh() const { return m_weaponsMesh; }

	bool IsInitialized() const { return m_initialized; }
	bool IsAttacking() const { return m_attacking; }

private:
	bool m_initialized;
	CDoomRPG *m_doomrpg;
	CTurnBasedCamera m_camera;
	bool m_attacking;
	unsigned int m_health, m_maxHealth;
	unsigned int m_armor, m_maxArmor;
	unsigned int m_credits, m_maxCredits;
	unsigned int m_ammo[5];
	unsigned int m_smallMedkits;
	unsigned int m_largeMedkits;
	unsigned int m_soulSpheres;
	unsigned int m_berserkers;
	unsigned int m_dogCollars;
	CKey *m_keys[4];
	CWeapon *m_weapons[12];
	unsigned int m_currentWeapon;
	CInputAction *m_action, *m_weaponAction[9];
	std::shared_ptr<CAnimatedMeshStateful> m_weaponsMesh;
	CCollisionHandler m_collisionHandler;
	CWeapon *m_pistol;
	std::shared_ptr<CSound> m_itemPickup;
	std::shared_ptr<CSound> m_weaponPickup;
	std::shared_ptr<CSound> m_powerupPickup;
	std::shared_ptr<CSound> m_noUse;
};
#endif