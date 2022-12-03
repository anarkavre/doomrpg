#include "glm/gtc/matrix_access.hpp"
#include "glm/gtx/transform.hpp"

#include "CCamera.h"
#include "Game/CGame.h"

CCamera::~CCamera()
{
	if (m_initialized)
		m_game->GetCameraSystem().UnregisterCamera(*this);
}

bool CCamera::Initialize(CGame *game, float hfov, float aspect, float near, float far, const glm::vec3 &position, const glm::vec3 &rotation, float speed, float angularSpeed)
{
	if (!m_initialized && game != nullptr)
	{
		m_game = game;

		m_hfov = hfov;
		m_aspect = aspect;
		m_near = near;
		m_far = far;
		SetPosition(position);
		SetRotation(rotation);
		SetOldPosition(position);
		SetOldRotation(rotation);
		m_speed = speed;
		m_angularSpeed = angularSpeed;
		m_updatePerspectiveMatrix = true;
		m_updateViewMatrix = true;

		m_game->GetCameraSystem().RegisterCamera(*this);

		m_initialized = true;
	}

	return m_initialized;
}

bool CCamera::Update(float elapsedTime)
{
	if (!m_initialized)
		return false;

	std::shared_ptr<CObject> child = GetChild();

	if (child != nullptr)
	{
		child->SetPosition(GetPosition());
		child->SetRotation(GetRotation());
		child->SetOldPosition(GetOldPosition());
		child->SetOldRotation(GetOldRotation());
		child->Update(elapsedTime);
		SetPosition(child->GetPosition());
		SetRotation(child->GetRotation());
	}

	return false;
}

void CCamera::UpdatePerspectiveMatrix()
{
	if (!m_initialized)
		return;

	if (m_updatePerspectiveMatrix)
	{
		m_vfov = glm::degrees(2 * glm::atan(glm::tan(glm::radians(m_hfov / 2)) / m_aspect));
		m_perspectiveMatrix = glm::perspective(glm::radians(m_vfov), m_aspect, m_near, m_far);
		m_updatePlanes = true;
		m_updatePerspectiveMatrix = false;
	}
}

void CCamera::UpdateViewMatrix()
{
	if (!m_initialized)
		return;

	if (m_updateViewMatrix)
	{
		UpdateTransformationMatrix();

		m_viewMatrix = glm::lookAt(GetPosition(), GetPosition() + GetForwardVector(), GetUpVector());
		m_updatePlanes = true;
		m_updateViewMatrix = false;
	}
}

void CCamera::UpdatePlanes()
{
	if (!m_initialized)
		return;

	if (m_updatePlanes)
	{
		glm::mat4 MVPMatrix = m_perspectiveMatrix * m_viewMatrix;
		m_planes[0] = ComputePlane(glm::row(MVPMatrix, 2) + glm::row(MVPMatrix, 3));
		m_planes[1] = ComputePlane(-glm::row(MVPMatrix, 2) + glm::row(MVPMatrix, 3));
		m_planes[2] = ComputePlane(glm::row(MVPMatrix, 1) + glm::row(MVPMatrix, 3));
		m_planes[3] = ComputePlane(-glm::row(MVPMatrix, 1) + glm::row(MVPMatrix, 3));
		m_planes[4] = ComputePlane(glm::row(MVPMatrix, 0) + glm::row(MVPMatrix, 3));
		m_planes[5] = ComputePlane(-glm::row(MVPMatrix, 0) + glm::row(MVPMatrix, 3));
		m_updatePlanes = false;
	}
}

void CCamera::UpdateCamera()
{
	if (!m_initialized)
		return;

	UpdatePerspectiveMatrix();
	UpdateViewMatrix();
	UpdatePlanes();
}

bool CCamera::IsBoxInFrustum(const glm::vec3 &bboxMin, const glm::vec3 &bboxMax) const
{
	if (!m_initialized)
		return false;

	bool result = true;
	glm::vec3 positive, negative;

	for (unsigned int i = 0; i < 6; i++)
	{
		const glm::vec4 &plane = m_planes[i];

		positive.x = bboxMin.x;
		positive.y = bboxMin.y;
		positive.z = bboxMin.z;

		if (plane.x >= 0.0f)
			positive.x = bboxMax.x;

		if (plane.y >= 0.0f)
			positive.y = bboxMax.y;

		if (plane.z >= 0.0f)
			positive.z = bboxMax.z;

		if (glm::dot(plane, glm::vec4(positive, 1.0f)) < 0.0f)
			return false;

		negative.x = bboxMax.x;
		negative.y = bboxMax.y;
		negative.z = bboxMax.z;

		if (plane.x >= 0.0f)
			negative.x = bboxMin.x;

		if (plane.y >= 0.0f)
			negative.y = bboxMin.y;

		if (plane.z >= 0.0f)
			negative.z = bboxMin.z;

		if (glm::dot(plane, glm::vec4(negative, 1.0f)) < 0.0f)
			result = true;
	}

	return result;
}