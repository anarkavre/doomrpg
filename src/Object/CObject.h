#ifndef __COBJECT_H__
#define __COBJECT_H__

#include <memory>
#include <string>

#include "glm/glm.hpp"

#include "Interface/ICollisionHandler.h"
#include "Interface/IUpdateHandler.h"

enum class ECObjectType
{
	Object,
	Camera,
	Mesh
};

class CObject : public IUpdateHandler
{
public:
	CObject(ECObjectType type = ECObjectType::Object) : m_type(type) { UpdateTransformationMatrix(); }

	bool Update(float elapsedTime);
	void UpdateTransformationMatrix();
	void RegisterCollisionHandler(ICollisionHandler &collisionHandler) { m_collisionHandler = &collisionHandler; }
	void UnregisterCollisionHandler() { m_collisionHandler = nullptr; }

	const std::string &GetName() const { return m_name; }
	ECObjectType GetType() const { return m_type; }
	const glm::vec3 &GetPosition() const { return m_position; }
	void SetPosition(const glm::vec3 &position) { m_position = position; m_updateTransformationMatrix = true; if (m_child != nullptr) m_child->SetPosition(position); }
	const glm::vec3 &GetRotation() const { return m_rotation; }
	void SetRotation(const glm::vec3 &rotation) { m_rotation = rotation; m_updateTransformationMatrix = true; if (m_child != nullptr) m_child->SetRotation(rotation); }
	const glm::vec3 &GetOldPosition() const { return m_oldPosition; }
	void SetOldPosition(const glm::vec3 &oldPosition) { m_oldPosition = oldPosition; if (m_child != nullptr) m_child->SetOldPosition(oldPosition); }
	const glm::vec3 &GetOldRotation() const { return m_oldRotation; }
	void SetOldRotation(const glm::vec3 &oldRotation) { m_oldRotation = oldRotation; if (m_child != nullptr) m_child->SetOldRotation(oldRotation); }
	const glm::vec3 &GetOffset() const { return m_offset; }
	void SetOffset(const glm::vec3 &offset) { m_offset = offset; m_updateTransformationMatrix = true; }
	const glm::vec3 &GetScale() const { return m_scale; }
	void SetScale(const glm::vec3 &scale) { m_scale = scale; m_updateTransformationMatrix = true; }
	float GetX() const { return m_position.x; }
	void SetX(float x) { m_position.x = x; m_updateTransformationMatrix = true; if (m_child != nullptr) m_child->SetX(x); }
	float GetY() const { return m_position.y; }
	void SetY(float y) { m_position.y = y; m_updateTransformationMatrix = true; if (m_child != nullptr) m_child->SetY(y); }
	float GetZ() const { return m_position.z; }
	void SetZ(float z) { m_position.z = z; m_updateTransformationMatrix = true; if (m_child != nullptr) m_child->SetZ(z); }
	float GetAngleX() const { return m_rotation.x; }
	void SetAngleX(float x) { m_rotation.x = x; m_updateTransformationMatrix = true; if (m_child != nullptr) m_child->SetAngleX(x); }
	float GetAngleY() const { return m_rotation.y; }
	void SetAngleY(float y) { m_rotation.y = y; m_updateTransformationMatrix = true; if (m_child != nullptr) m_child->SetAngleY(y); }
	float GetAngleZ() const { return m_rotation.z; }
	void SetAngleZ(float z) { m_rotation.z = z; m_updateTransformationMatrix = true; if (m_child != nullptr) m_child->SetAngleZ(z); }
	float GetOldX() const { return m_oldPosition.x; }
	void SetOldX(float x) { m_oldPosition.x = x; if (m_child != nullptr) m_child->SetOldX(x); }
	float GetOldY() const { return m_oldPosition.y; }
	void SetOldY(float y) { m_oldPosition.y = y; if (m_child != nullptr) m_child->SetOldY(y); }
	float GetOldZ() const { return m_oldPosition.z; }
	void SetOldZ(float z) { m_oldPosition.z = z; if (m_child != nullptr) m_child->SetOldZ(z); }
	float GetOldAngleX() const { return m_oldRotation.x; }
	void SetOldAngleX(float x) { m_oldRotation.x = x; if (m_child != nullptr) m_child->SetOldAngleX(x); }
	float GetOldAngleY() const { return m_oldRotation.y; }
	void SetOldAngleY(float y) { m_oldRotation.y = y; if (m_child != nullptr) m_child->SetOldAngleY(y); }
	float GetOldAngleZ() const { return m_oldRotation.z; }
	void SetOldAngleZ(float z) { m_oldRotation.z = z; if (m_child != nullptr) m_child->SetOldAngleZ(z); }
	float GetScaleX() const { return m_scale.x; }
	void SetScaleX(float x) { m_scale.x = x; m_updateTransformationMatrix = true; }
	float GetScaleY() const { return m_scale.y; }
	void SetScaleY(float y) { m_scale.y = y; m_updateTransformationMatrix = true; }
	float GetScaleZ() const { return m_scale.z; }
	void SetScaleZ(float z) { m_scale.z = z; m_updateTransformationMatrix = true; }
	float GetOffsetX() const { return m_offset.x; }
	void SetOffsetX(float x) { m_offset.x = x; m_updateTransformationMatrix = true; }
	float GetOffsetY() const { return m_offset.y; }
	void SetOffsetY(float y) { m_offset.y = y; m_updateTransformationMatrix = true; }
	float GetOffsetZ() const { return m_offset.z; }
	void SetOffsetZ(float z) { m_offset.z = z; m_updateTransformationMatrix = true; }
	const glm::vec3 &GetUpVector() const { return m_upVector; }
	const glm::vec3 &GetForwardVector() const { return m_forwardVector; }
	const glm::vec3 &GetRightVector() const { return m_rightVector; }
	const glm::mat4 &GetTranslationMatrix() const { return m_translationMatrix; }
	const glm::mat4 &GetRotationMatrix() const { return m_rotationMatrix; }
	const glm::mat4 &GetScalingMatrix() const { return m_scalingMatrix; }
	const glm::mat4 &GetOffsetMatrix() const { return m_offsetMatrix; }
	const glm::mat4 &GetTransformationMatrix() const { return m_transformationMatrix; }
	ICollisionHandler *GetCollisionHandler() const { return m_collisionHandler; }
	CObject *GetParent() const { return m_parent; }
	std::shared_ptr<CObject> GetChild() const { return m_child; }
	void SetChild(std::shared_ptr<CObject> child) { if (m_child != nullptr) m_child->m_parent = nullptr; m_child = child; if (m_child != nullptr) { m_child->m_parent = this; m_child->SetPosition(m_position); m_child->SetRotation(m_rotation); m_child->SetOldPosition(m_oldPosition); m_child->SetOldRotation(m_oldRotation); } }
	CObject *GetTarget() const { return m_target; }
	void SetTarget(CObject *target) { m_target = target; }
	bool GetHidden() const { return m_hidden; }
	void SetHidden(bool hidden) { m_hidden = hidden; }

	bool IsHidden() const { return m_hidden; }

protected:
	void SetName(const std::string &name) { m_name = name; }
	void SetType(ECObjectType type) { m_type = type; }

private:
	std::string m_name;
	ECObjectType m_type{ECObjectType::Object};
	glm::vec3 m_position{0.0f}, m_oldPosition{0.0f};
	glm::vec3 m_rotation{0.0f}, m_oldRotation{0.0f};
	glm::vec3 m_scale{1.0f};
	glm::vec3 m_offset{0.0f};
	glm::vec3 m_upVector{0.0f}, m_forwardVector{0.0f}, m_rightVector{0.0f};
	glm::mat4 m_translationMatrix{0.0f}, m_rotationMatrix{0.0f}, m_scalingMatrix{0.0f}, m_offsetMatrix{0.0f}, m_transformationMatrix{0.0f};
	bool m_updateTransformationMatrix{true};
	ICollisionHandler *m_collisionHandler{nullptr};
	CObject *m_parent{nullptr};
	std::shared_ptr<CObject> m_child;
	CObject *m_target{nullptr};
	bool m_hidden{false};
};

#endif