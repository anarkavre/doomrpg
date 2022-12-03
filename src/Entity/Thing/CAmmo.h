#ifndef __CAMMO_H__
#define __CAMMO_H__

#include "CThing.h"

class CAmmo : public CThing
{
public:
	CAmmo() {}
	CAmmo(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing) {}

	bool Pickup();
};

#endif