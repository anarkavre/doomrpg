#include "glm/gtx/transform.hpp"

#include "CTurnBasedCamera.h"
#include "Game/CGame.h"
#include "Object/CObject.h"
#include "Window/CWindow.h"

CTurnBasedCamera::~CTurnBasedCamera()
{
	if (m_game != nullptr)
		m_game->GetGameLoop().UnregisterEventHandler(*this, m_game->GetWindow().GetID());
}

bool CTurnBasedCamera::Initialize(CGame *game, float hfov, float aspect, float near, float far, const glm::vec3 &position, const glm::vec3 &rotation, float speed, float angularSpeed)
{
	if (CCamera::Initialize(game, hfov, aspect, near, far, position, rotation, speed, angularSpeed))
	{
		m_actions[0] = &m_game->GetInputSystem().RegisterAction("moveForward");
		m_actions[1] = &m_game->GetInputSystem().RegisterAction("moveBackward");
		m_actions[2] = &m_game->GetInputSystem().RegisterAction("moveLeft");
		m_actions[3] = &m_game->GetInputSystem().RegisterAction("moveRight");
		m_actions[4] = &m_game->GetInputSystem().RegisterAction("turnLeft");
		m_actions[5] = &m_game->GetInputSystem().RegisterAction("turnRight");

		m_game->GetInputSystem().BindAction("moveForward", SDLK_w);
		m_game->GetInputSystem().BindAction("moveBackward", SDLK_s);
		m_game->GetInputSystem().BindAction("moveLeft", SDLK_a);
		m_game->GetInputSystem().BindAction("moveRight", SDLK_d);
		m_game->GetInputSystem().BindAction("turnLeft", SDLK_LEFT);
		m_game->GetInputSystem().BindAction("turnRight", SDLK_RIGHT);

		m_game->GetGameLoop().RegisterEventHandler(*this, m_game->GetWindow().GetID());
	}

	return CCamera::IsInitialized();
}

bool CTurnBasedCamera::ProcessEvent(const SDL_Event &event)
{
	if (!CCamera::IsInitialized())
		return false;

	if (SDL_IsTextInputActive())
		return false;

	switch (event.type)
	{
	case SDL_CONTROLLERAXISMOTION:
		if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
		{
			if (event.caxis.value < -16384)
				m_actions[2]->SetPressed(true);
			else
				m_actions[2]->SetPressed(false);

			if (event.caxis.value > 16383)
				m_actions[3]->SetPressed(true);
			else
				m_actions[3]->SetPressed(false);
		}
		else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
		{
			if (event.caxis.value < -16384)
				m_actions[0]->SetPressed(true);
			else
				m_actions[0]->SetPressed(false);

			if (event.caxis.value > 16383)
				m_actions[1]->SetPressed(true);
			else
				m_actions[1]->SetPressed(false);
		}
		else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
		{
			if (event.caxis.value < -16384)
				m_actions[4]->SetPressed(true);
			else
				m_actions[4]->SetPressed(false);

			if (event.caxis.value > 16383)
				m_actions[5]->SetPressed(true);
			else
				m_actions[5]->SetPressed(false);
		}
		break;
	default:
		break;
	}

	return false;
}

bool CTurnBasedCamera::Update(float elapsedTime)
{
	if (!CCamera::IsInitialized() || m_locked)
		return false;

	if (m_actions[0]->IsPressed() || m_actions[1]->IsPressed() || m_actions[2]->IsPressed() || m_actions[3]->IsPressed() || m_actions[4]->IsPressed() || m_actions[5]->IsPressed())
	{
		if (!m_moving && !m_turning)
		{
			glm::vec3 moveVector = glm::vec3(0.0f);
			float turnDirection = 0.0f;

			if (m_actions[0]->IsPressed())
				moveVector += glm::vec3(0.0f, 0.0f, -1.0f);
			else if (m_actions[1]->IsPressed())
				moveVector -= glm::vec3(0.0f, 0.0f, -1.0f);
			else if (m_actions[2]->IsPressed())
				moveVector -= glm::vec3(1.0f, 0.0f, 0.0f);
			else if (m_actions[3]->IsPressed())
				moveVector += glm::vec3(1.0f, 0.0f, 0.0f);
			else if (m_actions[4]->IsPressed())
				turnDirection = 1.0f;
			else if (m_actions[5]->IsPressed())
				turnDirection = -1.0f;

			if (moveVector != glm::vec3(0.0f))
			{
				moveVector = glm::normalize(moveVector);
				m_directionVector = glm::vec3(glm::rotate(glm::radians(GetAngleY()), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(moveVector, 1.0f));
				SetOldPosition(GetPosition());
				m_moving = true;
			}
			else if (turnDirection != 0.0f)
			{
				m_turnDirection = turnDirection;
				SetOldRotation(GetRotation());
				m_turning = true;
			}
		}

		m_actions[0]->SetPressed(false);
		m_actions[1]->SetPressed(false);
		m_actions[2]->SetPressed(false);
		m_actions[3]->SetPressed(false);
		m_actions[4]->SetPressed(false);
		m_actions[5]->SetPressed(false);
	}

	if (m_moving)
	{
		m_time = glm::min(m_time + elapsedTime, 1.0f);

		SetPosition(GetOldPosition() + m_speed * m_time * m_directionVector);

		std::shared_ptr<CObject> child = GetChild();

		if (child != nullptr)
		{
			child->SetPosition(GetPosition());
			child->SetOldPosition(GetOldPosition());

			ICollisionHandler *collisionHandler = child->GetCollisionHandler();

			if (collisionHandler != nullptr)
			{
				collisionHandler->HandleCollision(child.get());

				SetPosition(child->GetPosition());

				if (GetPosition() == GetOldPosition())
				{
					m_time = 0.0f;
					m_moving = false;
				}
			}

			child->UpdateTransformationMatrix();
		}

		if (m_time == 1.0f)
		{
			m_time = 0.0f;
			m_moving = false;
		}

		m_updateViewMatrix = true;
	}
	else if (m_turning)
	{
		m_time = glm::min(m_time + elapsedTime, 1.0f);

		SetAngleY(GetOldAngleY() + m_angularSpeed * m_time * m_turnDirection);

		if (GetAngleY() < 0.0f)
			SetAngleY(GetAngleY() + 360.0f);
		else if (GetAngleY() >= 360.0f)
			SetAngleY(GetAngleY() - 360.0f);

		std::shared_ptr<CObject> child = GetChild();

		if (child != nullptr)
		{
			child->SetRotation(GetRotation());
			child->SetOldRotation(GetOldRotation());
			child->UpdateTransformationMatrix();
		}

		if (m_time == 1.0f)
		{
			m_time = 0.0f;
			m_turning = false;
		}

		m_updateViewMatrix = true;
	}

	return false;
}