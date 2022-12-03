#include "glm/gtx/transform.hpp"

#include "CObject.h"

bool CObject::Update(float elapsedTime)
{
	if (m_collisionHandler != nullptr)
		m_collisionHandler->HandleCollision(this);

	UpdateTransformationMatrix();

	return false;
}

void CObject::UpdateTransformationMatrix()
{
	if (m_updateTransformationMatrix)
	{
		m_translationMatrix = glm::translate(m_position);
		m_rotationMatrix = glm::rotate(glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		m_scalingMatrix = glm::scale(m_scale);
		m_offsetMatrix = glm::translate(m_offset);
		m_upVector = glm::vec3(m_rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		m_forwardVector = glm::vec3(m_rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
		m_rightVector = glm::cross(m_forwardVector, m_upVector);
		m_transformationMatrix = m_translationMatrix * m_rotationMatrix * m_scalingMatrix * m_offsetMatrix;
		m_updateTransformationMatrix = false;

		if (m_child != nullptr)
			m_child->UpdateTransformationMatrix();
	}
}