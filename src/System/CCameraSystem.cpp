#include "CCameraSystem.h"
#include "Game/CGame.h"
#include "Window/CWindow.h"

CCameraSystem::~CCameraSystem()
{
	if (m_initialized)
		m_game->GetGameLoop().UnregisterSystem(*this);
}

bool CCameraSystem::Initialize(CGame *game)
{
	if (ISystem::Initialize(game))
	{
		m_game->GetGameLoop().RegisterSystem(*this);

		m_defaultCamera.Initialize(m_game, 90.0f, static_cast<float>(m_game->GetWindow().GetWidth()) / static_cast<float>(m_game->GetWindow().GetHeight()), 0.001f, 1000.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, 90.0f);
		m_viewCamera = &m_defaultCamera;
		m_game->GetGameLoop().RegisterUpdateHandler(*m_viewCamera);

		m_game->GetConsole().RegisterVariable("fov", m_viewCamera->GetHorizontalFov(), [this](float &value) { m_viewCamera->SetHorizontalFov(value); });

		m_initialized = true;
	}

	return m_initialized;
}

bool CCameraSystem::Update()
{
	for (auto it = m_cameras.begin(); it != m_cameras.end(); ++it)
		(*it)->UpdateCamera();

	return false;
}

void CCameraSystem::RegisterCamera(CCamera &camera)
{
	if (find(m_cameras.begin(), m_cameras.end(), &camera) != m_cameras.end())
		return;

	m_cameras.push_back(&camera);
}

void CCameraSystem::UnregisterCamera(CCamera &camera)
{
	m_cameras.remove(&camera);
}

void CCameraSystem::ResetDefaultCamera()
{
	m_defaultCamera.SetHorizontalFov(90.0f);
	m_defaultCamera.SetAspectRatio(static_cast<float>(m_game->GetWindow().GetWidth()) / static_cast<float>(m_game->GetWindow().GetHeight()));
	m_defaultCamera.SetNearPlaneDistance(0.001f);
	m_defaultCamera.SetFarPlaneDistance(1000.0f);
	m_defaultCamera.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	m_defaultCamera.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_defaultCamera.SetOldPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	m_defaultCamera.SetOldRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_defaultCamera.SetSpeed(100.0f);
	m_defaultCamera.SetAngularSpeed(90.0f);
	m_defaultCamera.SetChild(nullptr);
}

void CCameraSystem::ChangeViewCamera(CCamera &viewCamera)
{
	m_game->GetGameLoop().UnregisterUpdateHandler(*m_viewCamera);
	m_game->GetGameLoop().RegisterUpdateHandler(viewCamera);
}