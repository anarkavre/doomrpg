#ifndef __CGAMEPLAYSTATE_H__
#define __CGAMEPLAYSTATE_H__

#include <vector>

#include "Game/CGameState.h"

class CDoomRPG;
class CSound;

struct Event;

class CGameplayState : public CGameState
{
public:
	CGameplayState(CGame *game = nullptr) { Initialize(game); }

	bool Initialize(CGame *game);
	void Update();
	bool RunActionEvent(Event *event);
	bool RunEnterEvent(Event *event, unsigned int direction);
	bool RunLeaveEvent(Event *event, unsigned int direction);
	bool RunLookEvent(Event *event, unsigned int direction);

private:
	CDoomRPG *m_doomrpg{nullptr};
	std::vector<unsigned int> m_nodeIndices;
	std::shared_ptr<CSound> m_noUse;
};

#endif