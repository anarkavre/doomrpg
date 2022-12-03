#ifndef __CCREDIT_H__
#define __CCREDIT_H__

#include "CThing.h"

class CCredit : public CThing
{
public:
	CCredit() {}
	CCredit(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing) {}

	bool Pickup();
};

#endif