#include "CDoomRPG.h"
#include "Resource/CMap.h"
#include "Resource/CTexture.h"
#include "doomrpg_graphics.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

CDoomRPG::~CDoomRPG()
{
	FreeStrings(m_entityStrings);
	FreeStrings(m_baseStrings);
	FreeEntitiesEx(m_entitiesex);
	FreeEntities(m_entities);
	FreeMappings(m_mappings);
}

bool CDoomRPG::Initialize(int argc, char *argv[])
{
	if (!CGame::IsInitialized())
	{
		GetConsole().RegisterCommand("warp", [this](CConsole &console, const std::vector<std::string> &arguments) { if (!arguments.empty()) LoadMap(arguments[0]); });
		GetConsole().RegisterVariable("hud", 0, [this](int &value) { int hud = glm::clamp(value, 0, 1); value = hud; m_status.SetHUD(hud); });

		CGame::Initialize("Doom RPG", "doomrpg.png", "doomrpg.log", "doomrpg.cfg", argc, argv);

		m_isEx = (GetResourceManager().GetBasePath() == "data/brew/");

		m_mappings = LoadMappings(m_isEx ? "data/brew/mappings.bin" : "data/j2me/mappings.bin");
		m_baseStrings = LoadStrings(m_isEx ? "data/brew/base.str" : "data/j2me/base.str");

		if (m_isEx)
		{
			m_entitiesex = LoadEntitiesEx("data/brew/entities.db");
			m_entities = nullptr;
			m_entityStrings = nullptr;

			for (unsigned int i = 0; i < m_entitiesex->entityCount; i++)
			{
				entityex_t *entity = &m_entitiesex->entities[i];
				m_entitiesMap.insert(std::make_pair(entity->id, (entity_t *)entity));
				m_entityStringsMap.insert(std::make_pair(entity->id, (char *)entity->name));
			}
		}
		else
		{
			m_entities = LoadEntities("data/j2me/entities.db");
			m_entityStrings = LoadStrings("data/j2me/entities.str");

			for (unsigned int i = 0; i < m_entities->entityCount; i++)
			{
				entity_t *entity = &m_entities->entities[i];
				m_entitiesMap.insert(std::make_pair(entity->id, entity));
				m_entityStringsMap.insert(std::make_pair(entity->id, m_entityStrings->strings[i]));
			}
		}

		uint8_t *wallTexels = LoadTexels(m_isEx ? "data/brew/wtexels.bin" : "data/j2me/wtexels.bin");
		uint8_t *bitShapes = LoadBitShapes(m_isEx ? "data/brew/bitshapes.bin" : "data/j2me/bitshapes.bin");
		uint8_t *spriteTexels = LoadTexels(m_isEx ? "data/brew/stexels.bin" : "data/j2me/stexels.bin");
		uint16_t *palettes = LoadPalettes(m_isEx ? "data/brew/palettes.bin" : "data/j2me/palettes.bin");

		uint32_t *image[2];
		uint32_t imageWidth[2];
		uint32_t imageHeight[2];

		image[0] = CreateRGBAImageFromWallTextures(m_mappings->textureMappings, m_mappings->textureMappingCount, wallTexels, palettes, &imageWidth[0], &imageHeight[0]);
		image[1] = CreateRGBAImageFromSprites(m_mappings->spriteMappings, m_mappings->spriteMappingCount, bitShapes, spriteTexels, palettes, &imageWidth[1], &imageHeight[1]);

		FreeTexels(wallTexels);
		FreeBitShapes(bitShapes);
		FreeTexels(spriteTexels);
		FreePalettes(palettes);

		ECTextureFilter textureFilter = static_cast<ECTextureFilter>(GetConsole().GetVariable("r_texture_filter").GetInt());

		m_texture[0] = std::shared_ptr<CTexture>(std::make_shared<CTexture>(this, image[0], imageWidth[0], imageHeight[0], imageWidth[0] / 64, imageHeight[0] / 64));
		m_texture[0]->GenerateMipmaps();
		m_texture[0]->SetFilter(textureFilter);
		m_texture[0]->SetWrap(ECTextureCoord::V, ECTextureWrap::Clamp);

		m_texture[1] = std::shared_ptr<CTexture>(std::make_shared<CTexture>(this, image[1], imageWidth[1], imageHeight[1], imageWidth[1] / 64, imageHeight[1] / 64));
		m_texture[1]->GenerateMipmaps();
		m_texture[1]->SetFilter(textureFilter);
		m_texture[1]->SetWrap(ECTextureWrap::Clamp);

		free(image[0]);
		free(image[1]);

		m_player.Initialize(this);
		m_player.SetHealth(30);
		m_player.SetMaxHealth(30);
		m_player.SetArmor(0);
		m_player.SetMaxArmor(30);
		m_player.SetCredits(0);
		m_player.SetMaxCredits(UINT_MAX);

		LoadMap("intro.bsp");

		m_status.Initialize(this);
		m_dialog.Initialize(this);

		m_gameplayState.Initialize(this);
		SetGameState(&m_gameplayState);
	}

	return CGame::IsInitialized();
}

void CDoomRPG::LoadMap(const std::string &filename)
{
	if (!CGame::IsInitialized())
		return;

	m_map = GetResourceManager().AcquireResource<CMap>(filename);

	m_player.GetCamera().SetPosition(m_map->GetPlayerPosition());
	m_player.GetCamera().SetRotation(glm::vec3(0.0f, m_map->GetPlayerAngle() - 90.0f, 0.0f));
	m_player.GetCamera().SetOldPosition(m_map->GetPlayerPosition());
	m_player.GetCamera().SetOldRotation(glm::vec3(0.0f, m_map->GetPlayerAngle() - 90.0f, 0.0f));
	GetSceneSystem().Add(m_player.GetCamera());

	static_cast<CCollisionHandler *>(m_player.GetWeaponsMesh()->GetCollisionHandler())->SetMap(m_map);
}