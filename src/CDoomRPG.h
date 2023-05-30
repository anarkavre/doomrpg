#ifndef __CDOOMRPG_H__
#define __CDOOMRPG_H__

#include <memory>
#include <unordered_map>

#include "Game/CGame.h"
#include "Player/CPlayer.h"
#include "State/CGameplayState.h"
#include "UI/CDialog.h"
#include "UI/CStatus.h"
#include "doomrpg_data.h"

class CMap;
class CTexture;

class CDoomRPG : public CGame
{
public:
	CDoomRPG() {}
	CDoomRPG(int argc, char *argv[]) { Initialize(argc, argv); }
	~CDoomRPG();

	bool Initialize(int argc = 0, char *argv[] = nullptr);
	void LoadMap(const std::string &filename);

	const mappings_t *GetMappings() const { return m_mappings; }
	const char *GetBaseString(unsigned int index) const { return m_baseStrings->strings[index]; }
	const entity_t *GetEntity(unsigned int id) const { return (m_entitiesMap.count(id) == 1 ? m_entitiesMap.at(id) : nullptr); }
	const char *GetEntityString(unsigned int id) const { return (m_entityStringsMap.count(id) == 1 ? m_entityStringsMap.at(id) : nullptr); }
	std::shared_ptr<CTexture> GetTextures() const { return m_texture[0]; }
	std::shared_ptr<CTexture> GetSprites() const { return m_texture[1]; }
	CPlayer &GetPlayer() { return m_player; }
	std::shared_ptr<CMap> GetMap() const { return m_map; }
	CStatus &GetStatus() { return m_status; }
	CDialog &GetDialog() { return m_dialog; }
	CGameplayState &GetGameplayState() { return m_gameplayState; }

	bool IsEx() const { return m_isEx; }

private:
	bool m_isEx{false};
	mappings_t *m_mappings{nullptr};
	entities_t *m_entities{nullptr};
	entitiesex_t *m_entitiesex{nullptr};
	strings_t *m_baseStrings{nullptr};
	strings_t *m_entityStrings{nullptr};
	std::unordered_map<unsigned int, entity_t *> m_entitiesMap;
	std::unordered_map<unsigned int, char *> m_entityStringsMap;
	std::shared_ptr<CTexture> m_texture[2];
	CPlayer m_player;
	std::shared_ptr<CMap> m_map;
	CStatus m_status;
	CDialog m_dialog;
	CGameplayState m_gameplayState;
};

#endif