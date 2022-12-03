#ifndef __CWEAPON_H__
#define __CWEAPON_H__

#include "CThing.h"

class CWeapon : public CThing
{
public:
	CWeapon() : m_firing(false) {}
	CWeapon(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing), m_firing(false) {}

	bool Update(float elapsedTime);
	bool Pickup();
	void Fire();

	unsigned int GetAmmo() const;
	void SetAmmo(unsigned int ammo);
	unsigned int GetAmmoUsed() const { return m_ammoUsed; }
	void SetAmmoUsed(unsigned int ammoUsed) { m_ammoUsed = ammoUsed; }

	bool IsFiring() const { return m_firing; }

private:
	unsigned int m_ammoType;
	unsigned int m_ammoUsed;
	bool m_firing;
};

#endif