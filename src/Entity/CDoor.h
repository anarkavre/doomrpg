#ifndef __CDOOR_H__
#define __CDOOR_H__

#include "CEntity.h"

class CMesh;
class CSound;

enum class ECDoorState
{
	Closed,
	Opening,
	Opened,
	Closing,
	Locked,
	Unlocked
};

class CDoor : public CEntity
{
public:
	CDoor() {}
	CDoor(CDoomRPG *doomrpg, const linesegment_t *line);

	void Open();
	void Close();
	bool Update(float elapsedTime);

	ECDoorState GetState() const { return m_state; }
	void SetState(ECDoorState state) { SetId(state != m_state && (GetId() & 0xA) == 0xA ? GetId() ^ 0x1 : GetId()); m_state = (state == ECDoorState::Unlocked ? ECDoorState::Closed : state); ToggleMesh(); }
	float GetOpenDelay() const { return static_cast<float>(m_openDelayTics / m_doomrpg->GetGameLoop().GetUpdateRate()); }
	void SetOpenDelay(float openDelay) { m_openDelayTics = static_cast<unsigned int>(openDelay * m_doomrpg->GetGameLoop().GetUpdateRate()); }
	float GetCloseDelay() const { return static_cast<float>(m_closeDelayTics / m_doomrpg->GetGameLoop().GetUpdateRate()); }
	void SetCloseDelay(float closeDelay) { m_closeDelayTics = static_cast<unsigned int>(closeDelay * m_doomrpg->GetGameLoop().GetUpdateRate()); }
	float GetSpeed() const { return m_speed; }
	void SetSpeed(float speed) { m_speed = speed; }

private:
	void ToggleMesh();

	bool m_open{false};
	ECDoorState m_state{ECDoorState::Closed};
	unsigned int m_openDelayTics{0};
	unsigned int m_closeDelayTics{0};
	unsigned int m_tics{0};
	float m_speed{2.0f};
	std::shared_ptr<CMesh> m_mesh[2];
	std::shared_ptr<CSound> m_sound[2];
};

#endif