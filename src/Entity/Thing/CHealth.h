#ifndef __CHEALTH_H__
#define __CHEALTH_H__

#include "CThing.h"

class CHealth : public CThing
{
public:
	CHealth() {}
	CHealth(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing) {}

	bool Pickup();
};

#endif