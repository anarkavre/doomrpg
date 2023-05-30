#ifndef __CGAMESTATE_H__
#define __CGAMESTATE_H__

class CGame;

class CGameState
{
public:
	CGameState(CGame *game = nullptr) { Initialize(game); }

	virtual bool Initialize(CGame *game) { m_game = game; return (m_game != nullptr); }
	virtual void Update() {}

	bool IsInitialized() const { return m_initialized; }

protected:
	bool m_initialized{false};
	CGame *m_game{false};
};

#endif