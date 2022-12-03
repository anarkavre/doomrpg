#ifndef __CCAMERASYSTEM_H__
#define __CCAMERASYSTEM_H__

#include <list>

#include "Camera/CCamera.h"
#include "Interface/ISystem.h"

class CCamera;

class CCameraSystem : public ISystem
{
public:
	CCameraSystem(CGame *game = nullptr) { Initialize(game); }
	~CCameraSystem();

	bool Initialize(CGame *game);
	bool Update();
	void RegisterCamera(CCamera &camera);
	void UnregisterCamera(CCamera &camera);
	void ResetDefaultCamera();

	CCamera &GetDefaultCamera() { return m_defaultCamera; }
	CCamera &GetViewCamera() const { return *m_viewCamera; }
	void SetViewCamera(CCamera &viewCamera) { ChangeViewCamera(viewCamera); m_viewCamera = &viewCamera; }

private:
	void ChangeViewCamera(CCamera &viewCamera);

	std::list<CCamera *> m_cameras;
	CCamera m_defaultCamera;
	CCamera *m_viewCamera;
};
#endif