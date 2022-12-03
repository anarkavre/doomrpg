#ifndef __CARMOR_H__
#define __CARMOR_H__

#include "CThing.h"

class CArmor : public CThing
{
public:
	CArmor() {}
	CArmor(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing) {}

	bool Pickup();
};

#endif