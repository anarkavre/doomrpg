#ifndef __CMENUSTATE_H__
#define __CMENUSTATE_H__

#include "Game/CGameState.h"

class CDoomRPG;
class CMusic;

class CMenuState : public CGameState
{
public:
	CMenuState(CGame* game = nullptr) { Initialize(game); }

	bool Initialize(CGame* game);
	void Update();

private:
	CDoomRPG* m_doomrpg{nullptr};
	std::shared_ptr<CMusic> m_music;
};

#endif