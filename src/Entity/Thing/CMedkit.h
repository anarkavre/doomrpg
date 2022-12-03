#ifndef __CMEDKIT_H__
#define __CMEDIT_H__

#include "CThing.h"

class CMedkit : public CThing
{
public:
	CMedkit() {}
	CMedkit(CDoomRPG *doomrpg, const thing_t *thing) : CThing(doomrpg, thing) {}

	bool Pickup();
};

#endif