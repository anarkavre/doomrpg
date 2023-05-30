#ifndef __CSCENEENTITY_H__
#define __CSCENEENTITY_H__

#include <memory>
#include <vector>

class CGame;
class CObject;
class CTexture;

class CSceneEntity
{
public:
	CSceneEntity() {}
	CSceneEntity(const CSceneEntity &sceneEntity) = default;
	CSceneEntity(CSceneEntity &&sceneEntity) noexcept : m_initialized(std::exchange(sceneEntity.m_initialized, false)), m_game(sceneEntity.m_game), m_handle(sceneEntity.m_handle) {}
	CSceneEntity(CGame *game, std::shared_ptr<CObject> object, bool cameraDescendant = false) { Initialize(game, std::move(object), cameraDescendant); }
	~CSceneEntity();

	bool Initialize(CGame *game, std::shared_ptr<CObject> object, bool cameraDescendant = false);

	bool GetVisible() const;
	void SetVisible(bool visible);
	std::shared_ptr<CObject> GetObject() const;

	bool IsInitialized() const { return m_initialized; }
	bool IsVisible() const { return GetVisible(); }

private:
	bool m_initialized{false};
	CGame *m_game{nullptr};
	unsigned int m_handle{0};
};

#endif