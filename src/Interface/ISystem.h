#ifndef __ISYSTEM_H__
#define __ISYSTEM_H__

class CGame;

class ISystem
{
public:
	ISystem() : m_initialized(false), m_game(nullptr) {}
	virtual ~ISystem() {}

	virtual bool Initialize(CGame *game) { m_game = game; return (m_game != nullptr); }
	virtual bool Update() { return false; }

	bool IsInitialized() const { return m_initialized; }

protected:
	bool m_initialized;
	CGame *m_game;
};

#endif
