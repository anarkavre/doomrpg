#ifndef __CWEAPON_H__
#define __CWEAPON_H__

#include "CThing.h"

class CWeapon : public CThing
{
public:
	CWeapon() {}
	CWeapon(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing) {}

	bool Update(float elapsedTime);
	bool Pickup();
	void Fire();

	unsigned int GetMinStrength() const { return m_minStrength; };
	void SetMinStrength(unsigned int minStrength) { m_minStrength = minStrength; }
	unsigned int GetMaxStrength() const { return m_maxStrength; }
	void SetMaxStrength(unsigned int maxStrength) { m_maxStrength = maxStrength; }
	unsigned int GetRange() const { return m_range; }
	void SetRange(unsigned int range) { m_range = range; }
	unsigned int GetAccuracy() const { return m_accuracy; }
	void SetAccuracy(unsigned int accuracy) { m_accuracy = accuracy; }
	unsigned int GetAmmoType() const { return m_ammoType; }
	unsigned int GetAmmo() const;
	void SetAmmo(unsigned int ammo);
	unsigned int GetAmmoUsed() const { return m_ammoUsed; }
	void SetAmmoUsed(unsigned int ammoUsed) { m_ammoUsed = ammoUsed; }
	unsigned int GetAbsortionRatio() const { return m_absortionRatio; }
	void SetAbsortionRatio(unsigned int absortionRatio) { m_absortionRatio = absortionRatio; }

	bool IsFiring() const { return m_firing; }

private:
	unsigned int m_minStrength{0};
	unsigned int m_maxStrength{0};
	unsigned int m_range{0};
	unsigned int m_accuracy{0};
	unsigned int m_ammoType{0};
	unsigned int m_ammoUsed{0};
	unsigned int m_absortionRatio{0};
	bool m_firing{false};
};

#endif