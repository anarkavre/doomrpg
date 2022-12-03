#ifndef __CBERSERKER_H__
#define __CBERSERKER_H__

#include "CThing.h"

class CBerserker : public CThing
{
public:
	CBerserker() {}
	CBerserker(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing) {}

	bool Pickup();
};

#endif