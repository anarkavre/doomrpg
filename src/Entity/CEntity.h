#ifndef __CENTITY_H__
#define __CENTITY_H__

#include "CDoomRPG.h"
#include "Object/CObject.h"

class CEntity : public CObject
{
public:
	CEntity() {}
	CEntity(CDoomRPG *doomrpg, unsigned int id) : m_doomrpg(doomrpg), m_id(id), m_type(doomrpg->GetEntity(id) != nullptr ? doomrpg->GetEntity(id)->type : (id == 0xD8 ? 17 : (id > 0xDC ? 18 : 19))), m_name(doomrpg->GetEntityString(id)) {}

	unsigned int GetId() const { return m_id; }
	unsigned int GetType() const { return m_type; }
	const char *GetName() const { return m_name; }

protected:
	void SetId(unsigned int id) { m_id = id; m_type = (m_doomrpg->GetEntity(id) != nullptr ? m_doomrpg->GetEntity(id)->type : (id == 0xD8 ? 17 : (id > 0xDC ? 18 : 19))); m_name = m_doomrpg->GetEntityString(id); }

	CDoomRPG *m_doomrpg{nullptr};

private:
	unsigned int m_id{0};
	unsigned int m_type{0};
	const char *m_name{nullptr};
};

#endif