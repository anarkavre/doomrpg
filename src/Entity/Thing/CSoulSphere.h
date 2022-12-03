#ifndef __CSOULSPHERE_H__
#define __CSOULSPHERE_H__

#include "CThing.h"

class CSoulSphere : public CThing
{
public:
	CSoulSphere() {}
	CSoulSphere(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing) {}

	bool Pickup();
};

#endif