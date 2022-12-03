#ifndef __CTURNBASEDCAMERA_H__
#define __CTURNBASEDCAMERA_H__

#include "CCamera.h"
#include "Interface/IEventHandler.h"

class CInputAction;

class CTurnBasedCamera : public CCamera, public IEventHandler
{
public:
	CTurnBasedCamera() : m_locked(false), m_moving(false), m_turning(false), m_time(0.0f) {}
	CTurnBasedCamera(CGame *game, float hfov, float aspect, float near, float far, const glm::vec3 &position, const glm::vec3 &rotation, float speed, float angularSpeed) : m_locked(false), m_moving(false), m_turning(false), m_time(0.0f) { Initialize(game, hfov, aspect, near, far, position, rotation, speed, angularSpeed); }
	~CTurnBasedCamera();

	bool Initialize(CGame *game, float hfov, float aspect, float near, float far, const glm::vec3 &position, const glm::vec3 &rotation, float speed, float angularSpeed);
	bool ProcessEvent(const SDL_Event &event);
	bool Update(float elapsedTime);

	void Lock() { m_locked = true; }
	void Unlock() { m_locked = false; }

	bool IsLocked() const { return m_locked; }
	bool IsMoving() const { return m_moving; }
	bool IsTurning() const { return m_turning; }

private:
	CInputAction *m_actions[6];
	glm::vec3 m_directionVector;
	float m_turnDirection;
	bool m_locked;
	bool m_moving, m_turning;
	float m_time;
};

#endif