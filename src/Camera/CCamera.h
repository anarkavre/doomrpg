#ifndef __CCAMERA_H__
#define __CCAMERA_H__

#include "glm/glm.hpp"

#include "Object/CObject.h"

class CGame;

class CCamera : public CObject
{
public:
	CCamera() : CObject(ECObjectType::Camera) {}
	CCamera(CGame *game, float hfov, float aspect, float near, float far, const glm::vec3 &position, const glm::vec3 &rotation, float speed, float angularSpeed) : CObject(ECObjectType::Camera) { Initialize(game, hfov, aspect, near, far, position, rotation, speed, angularSpeed); }
	~CCamera();

	bool Initialize(CGame *game, float hfov, float aspect, float near, float far, const glm::vec3 &position, const glm::vec3 &rotation, float speed, float angularSpeed);
	bool Update(float elapsedTime);
	void UpdatePerspectiveMatrix();
	void UpdateViewMatrix();
	void UpdatePlanes();
	void UpdateCamera();
	bool IsBoxInFrustum(const glm::vec3 &bboxMin, const glm::vec3 &bboxMax) const;

	float GetHorizontalFov() const { return m_hfov; }
	void SetHorizontalFov(float hfov) { m_hfov = hfov; m_updatePerspectiveMatrix = true; }
	float GetVerticalFov() const { return m_vfov; }
	float GetAspectRatio() const { return m_aspect; }
	void SetAspectRatio(float aspect) { m_aspect = aspect; m_updatePerspectiveMatrix = true; }
	float GetNearPlaneDistance() const { return m_near; }
	void SetNearPlaneDistance(float near) { m_near = near; m_updatePerspectiveMatrix = true; }
	float GetFarPlaneDistance() const { return m_far; }
	void SetFarPlaneDistance(float far) { m_far = far; m_updatePerspectiveMatrix = true; }
	void SetPosition(const glm::vec3 &position) { CObject::SetPosition(position); m_updateViewMatrix = true; }
	void SetRotation(const glm::vec3 &rotation) { CObject::SetRotation(rotation); m_updateViewMatrix = true; }
	void SetX(float x) { CObject::SetX(x); m_updateViewMatrix = true; }
	void SetY(float y) { CObject::SetY(y); m_updateViewMatrix = true; }
	void SetZ(float z) { CObject::SetZ(z); m_updateViewMatrix = true; }
	void SetAngleX(float x) { CObject::SetAngleX(x); m_updateViewMatrix = true; }
	void SetAngleY(float y) { CObject::SetAngleY(y); m_updateViewMatrix = true; }
	void SetAngleZ(float z) { CObject::SetAngleZ(z); m_updateViewMatrix = true; }
	float GetSpeed() const { return m_speed; }
	void SetSpeed(float speed) { m_speed = speed; }
	float GetAngularSpeed() const { return m_angularSpeed; }
	void SetAngularSpeed(float angularSpeed) { m_angularSpeed = angularSpeed; }
	const glm::mat4 &GetPerspectiveMatrix() const { return m_perspectiveMatrix; }
	const glm::mat4 &GetViewMatrix() const { return m_viewMatrix; }
	const glm::vec4 *GetPlanes() const { return m_planes; }
	const glm::vec4 &GetNearPlane() const { return m_planes[0]; }
	const glm::vec4 &GetFarPlane() const { return m_planes[1]; }
	const glm::vec4 &GetBottomPlane() const { return m_planes[2]; }
	const glm::vec4 &GetTopPlane() const { return m_planes[3]; }
	const glm::vec4 &GetLeftPlane() const { return m_planes[4]; }
	const glm::vec4 &GetRightPlane() const { return m_planes[5]; }

	bool IsInitialized() const { return m_initialized; }

protected:
	CGame *m_game{nullptr};
	float m_hfov{0.0f}, m_vfov{0.0f};
	float m_aspect{0.0f};
	float m_near{0.0f}, m_far{0.0f};
	float m_speed{0.0f}, m_angularSpeed{0.0f};
	bool m_updatePerspectiveMatrix{false}, m_updateViewMatrix{false};

private:
	glm::vec4 ComputePlane(const glm::vec4 &coefficients) { return glm::vec4(coefficients) / glm::length(glm::vec3(coefficients)); }

	bool m_initialized{false};
	glm::mat4 m_perspectiveMatrix{0.0f}, m_viewMatrix{0.0f};
	glm::vec4 m_planes[6]{glm::vec4{0.0f}};
	bool m_updatePlanes{false};
};

#endif