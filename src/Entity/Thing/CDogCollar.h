#ifndef __CDOGCOLLAR_H__
#define __CDOGCOLLAR_H__

#include "CThing.h"

class CDogCollar : public CThing
{
public:
	CDogCollar() {}
	CDogCollar(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing) {}

	bool Pickup();
};

#endif