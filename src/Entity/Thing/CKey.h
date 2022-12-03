#ifndef __CKEY_H__
#define __CKEY_H__

#include "CThing.h"

class CKey : public CThing
{
public:
	CKey() {}
	CKey(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing) {}

	bool Pickup();
};

#endif