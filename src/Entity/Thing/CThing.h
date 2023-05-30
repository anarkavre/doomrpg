#ifndef __CTHING_H__
#define __CTHING_H__

#include "CDoomRPG.h"
#include "Entity/CEntity.h"

class CMesh;

class CThing : public CEntity
{
public:
	CThing() {}
	CThing(CDoomRPG *doomrpg, const thing_t *thing);

	virtual bool Pickup() { return false; }

	unsigned int GetState() const { return m_state; }
	void SetState(unsigned int state) { m_state = state; }

	void SetHidden(bool hidden);

private:
	unsigned int m_state{0};
	std::shared_ptr<CMesh> m_mesh;
};

#endif