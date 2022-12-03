#include <stdexcept>

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

#include "CDoomRPG.h"
#include "CMap.h"
#include "CTexture.h"
#include "Entity/CDoor.h"
#include "Entity/Thing/CAmmo.h"
#include "Entity/Thing/CArmor.h"
#include "Entity/Thing/CBerserker.h"
#include "Entity/Thing/CCredit.h"
#include "Entity/Thing/CDogCollar.h"
#include "Entity/Thing/CHealth.h"
#include "Entity/Thing/CKey.h"
#include "Entity/Thing/CMedkit.h"
#include "Entity/Thing/CSoulSphere.h"
#include "Entity/Thing/CWeapon.h"
#include "Geometry/CAnimatedQuadList.h"
#include "Mesh/CAnimatedMesh.h"
#include "UI/CStatus.h"

CMap::CMap(CGame *game, unsigned int handle, const std::string &name) : CResource(game, handle, name, GetResourcePath(), ECResourceType::Map, false)
{
	CConsole &console = m_game->GetConsole();

	CDoomRPG *doomrpg = static_cast<CDoomRPG *>(m_game);

	std::string filename = m_path + m_name;

	console.Write("Loading resource \"" + filename + "\"...");

	const uint8_t *blockMap;
	uint16_t eventCount;
	const uint32_t *events;
	const command_t *commands;

	if (doomrpg->IsEx())
	{
		m_mapex = LoadBspMapEx(filename.c_str());

		console.Write("OK\n");

		m_playerPosition = glm::vec3((m_mapex->header.playerPosition & 0x1F) + 0.5f, 0.0f, ((m_mapex->header.playerPosition >> 5) & 0x1F) + 0.5f);
		m_playerAngle = m_mapex->header.playerAngle * 180.0f / 128;

		blockMap = m_mapex->blockMap;
		eventCount = m_mapex->eventCount;
		events = m_mapex->events;
		commands = m_mapex->commands;

		m_strings = m_mapex->strings;
	}
	else
	{
		m_map = LoadBspMap(filename.c_str());

		console.Write("OK\n");

		m_playerPosition = glm::vec3((m_map->header.playerPosition & 0x1F) + 0.5f, 0.0f, ((m_map->header.playerPosition >> 5) & 0x1F) + 0.5f);
		m_playerAngle = m_map->header.playerAngle * 180.0f / 128;

		blockMap = m_map->blockMap;
		eventCount = m_map->eventCount;
		events = m_map->events;
		commands = m_map->commands;

		m_strings = LoadStrings((filename.substr(0, filename.length() - 3) + "str").c_str());
	}

	BuildMap();

	for (unsigned int y = 0; y < 32; y++)
	{
		for (unsigned int x = 0; x < 32; x++)
		{
			m_blockMap[y][x] = (blockMap[y * 8 + (x / 4)] >> ((x % 4) * 2)) & 0x3;
		}
	}

	for (unsigned int i = 0; i < eventCount; i++)
	{
		const uint32_t *event = &events[i];
		unsigned int blockX = *event & 0x1F;
		unsigned int blockY = (*event >> 5) & 0x1F;

		Event newEvent;
		newEvent.commandStart = (*event >> 10) & 0x1FF;
		newEvent.commandCount = (*event >> 19) & 0x3F;
		newEvent.currentCommand = 0;
		newEvent.variable = 0;
		newEvent.halted = false;

		for (unsigned int i = 0; i < newEvent.commandCount; i++)
		{
			const command_t *cmd = &commands[newEvent.commandStart + i];

			unsigned int variable = (cmd->argument2 >> 16) & 0xFFFF;

			if (cmd->argument2 & 0x100)
				newEvent.onActionCommands[variable].push_back(i);
			else if (cmd->argument2 & 0xF)
			{
				if (cmd->argument2 & 0x1)
					newEvent.onEnterCommands[variable][0].push_back(i);
				if (cmd->argument2 & 0x2)
					newEvent.onEnterCommands[variable][1].push_back(i);
				if (cmd->argument2 & 0x4)
					newEvent.onEnterCommands[variable][2].push_back(i);
				if (cmd->argument2 & 0x8)
					newEvent.onEnterCommands[variable][3].push_back(i);
			}
			else if (cmd->argument2 & 0xF0)
			{
				if (cmd->argument2 & 0x10)
					newEvent.onLeaveCommands[variable][0].push_back(i);
				if (cmd->argument2 & 0x20)
					newEvent.onLeaveCommands[variable][1].push_back(i);
				if (cmd->argument2 & 0x40)
					newEvent.onLeaveCommands[variable][2].push_back(i);
				if (cmd->argument2 & 0x80)
					newEvent.onLeaveCommands[variable][3].push_back(i);
			}
			else if (!(cmd->argument2 & 0xFFFF))
			{
				if (cmd->argument2 & 0x10000000)
					newEvent.onLookCommands[0].push_back(i);
				if (cmd->argument2 & 0x20000000)
					newEvent.onLookCommands[1].push_back(i);
				if (cmd->argument2 & 0x40000000)
					newEvent.onLookCommands[2].push_back(i);
				if (cmd->argument2 & 0x80000000)
					newEvent.onLookCommands[3].push_back(i);
			}
		}

		m_events.insert(std::make_pair((blockY << 8) | blockX, newEvent));
	}
}

CMap::~CMap()
{
	for (unsigned int i = 0; i < m_things.size(); i++)
		delete m_things[i];

	CDoomRPG *doomrpg = static_cast<CDoomRPG *>(m_game);

	if (doomrpg->IsEx())
	{
		FreeBspMapEx(m_mapex);
	}
	else
	{
		FreeBspMap(m_map);

		FreeStrings(m_strings);
	}
}

void CMap::ExecuteCommand(Event *event, unsigned int command)
{
	CDoomRPG *doomrpg = static_cast<CDoomRPG *>(m_game);

	const command_t *cmd;
	
	if (doomrpg->IsEx())
		cmd = &m_mapex->commands[event->commandStart + command];
	else
		cmd = &m_map->commands[event->commandStart + command];

	switch (cmd->id)
	{
	case 1:
	{
		CPlayer &player = doomrpg->GetPlayer();
		player.SetX((cmd->argument1 & 0x1F) + 0.5f);
		player.SetZ(((cmd->argument1 >> 5) & 0x1F) + 0.5f);
		player.SetAngleY(((cmd->argument1 >> 16) & 0xFF) * 180.0f / 128);
	}
		break;
	case 2:
		// Change Level
		break;
	case 3:
	{
		Event *event = &m_events[cmd->argument1];

		for (unsigned int i = 0; i < event->commandCount; i++)
			ExecuteCommand(event, i);
	}
		break;
	case 4:
	case 24:
	{
		CStatus &status = doomrpg->GetStatus();
		std::string message = m_strings->strings[cmd->argument1];

		if (cmd->id == 4)
			status.AddMessage(message);
		else
			status.SetMessage(message);
	}
		break;
	case 7:
	{
		CThing *thing = m_things[cmd->argument1 & 0xFFFF];
		thing->SetHidden(false);
		thing->SetState((cmd->argument1 >> 16) & 0xFFFF);
	}
		break;
	case 8:
	case 26:
	{
		event->halted = true;

		CDialog &dialog = doomrpg->GetDialog();
		std::string message = m_strings->strings[cmd->argument1];
		dialog.SetMessage(message);
		dialog.Show();

		if (cmd->id == 26)
			dialog.Lock();
	}
		break;
	case 9:
		// Get Map Data
		break;
	case 10:
		// Enter Passcode (Halt on failure)
		break;
	case 11:
	{
		Event *event = &m_events[cmd->argument1 & 0xFFFF];
		event->variable = (cmd->argument1 >> 16) & 0xFFFF;
	}
		break;
	case 12:
	{
		CDoor *door = static_cast<CDoor *>(m_entities[m_doors[cmd->argument1]]);
		door->SetState(ECDoorState::Locked);
	}
		break;
	case 13:
	{
		CDoor *door = static_cast<CDoor *>(m_entities[m_doors[cmd->argument1]]);
		door->SetState(ECDoorState::Unlocked);
	}
		break;
	case 15:
	{
		CDoor *door = static_cast<CDoor *>(m_entities[m_doors[cmd->argument1]]);
		
		if (door->GetState() != ECDoorState::Locked)
			door->Open();
	}
		break;
	case 16:
	{
		CDoor *door = static_cast<CDoor *>(m_entities[m_doors[cmd->argument1]]);

		if (door->GetState() == ECDoorState::Opened)
			door->Close();
	}
		break;
	case 18:
	{
		auto range = m_thingLocations.equal_range(cmd->argument1);

		for_each(range.first, range.second,
			[this](std::unordered_multimap<unsigned int, unsigned int>::value_type &value)
			{
				CThing *thing = m_things[value.second];
				thing->SetHidden(true);
			}
		);
	}
		break;
	case 19:
	{
		Event *event = &m_events[cmd->argument1];

		if (event->variable == 0)
			event->variable++;
		else
			event->variable *= 2;
	}
		break;
	case 21:
		// Increase Status Item Count
		break;
	case 22:
		// Decrease Status Item Count
		break;
	case 23:
		// Show Message and Halt IF Status Item Count is less than Amount
		break;
	case 25:
		// Explosion
		break;
	case 27:
		// Change Level Parameters (called before command 2)
		break;
	case 29:
		// Shake Effect
		break;
	case 30:
		// Set Floor Color
		break;
	case 31:
		// Set Ceiling Color
		break;
	case 32:
		// Give/Take All Collected Weapons
		break;
	case 33:
		// Open Store
		break;
	case 34:
		// Set State of Thing at course grid Location (used only to kill humans)
		break;
	case 35:
		// Particle Effect
		break;
	case 39:
		// Show message IF [some level] not complete
		break;
	case 40:
		// Add Notebook Entry
		break;
	case 41:
		// Halt IF missing keycard
		break;
	default:
		break;
	}
}

void CMap::BuildMap()
{
	CDoomRPG *doomrpg = static_cast<CDoomRPG *>(m_game);

	const mappings_t *mappings = doomrpg->GetMappings();

	uint16_t nodeCount;
	const bspnode_t *nodes;
	uint16_t thingCount;
	const thing_t *things;

	if (doomrpg->IsEx())
	{
		nodeCount = m_mapex->nodeCount;
		nodes = m_mapex->nodes;
		thingCount = m_mapex->thingCount;
		things = m_mapex->things;
	}
	else
	{
		nodeCount = m_map->nodeCount;
		nodes = m_map->nodes;
		thingCount = m_map->thingCount;
		things = m_map->things;
	}

	m_game->GetSceneSystem().SetScenePartitionTree(m_bspTree);

	unsigned int meshId = UINT_MAX;
	std::string name = m_name.substr(0, m_name.find_last_of('.'));

	std::unordered_map<unsigned int, std::shared_ptr<CAnimatedQuadList>> wallFloorCeilingLookup;
	std::unordered_map<unsigned int, std::shared_ptr<CAnimatedQuadList>> decorationLookup;
	std::unordered_map<unsigned int, std::shared_ptr<CAnimatedQuadList>> fenceLookup;
	std::unordered_map<unsigned int, std::shared_ptr<CAnimatedQuadList>> lightLookup;
	std::vector<std::pair<const linesegment_t *, unsigned int>> lines;

	for (unsigned int i = 0; i < nodeCount; i++)
	{
		const bspnode_t *node = &nodes[i];

		BSPNode bspNode;

		if (node->nodeType == 0)
		{
			bspNode.type = ECBSPNodeType::Leaf;
			bspNode.bboxMin = glm::vec3(node->boundingBoxMin.x * 8 / 64.0f, -0.5f, node->boundingBoxMin.y * 8 / 64.0f);
			bspNode.bboxMax = glm::vec3(node->boundingBoxMax.x * 8 / 64.0f, 0.5f, node->boundingBoxMax.y * 8 / 64.0f);
			bspNode.splitPosition = 0.0f;
			bspNode.left = 0;
			bspNode.right = 0;

			unsigned int nodeIndex = m_bspTree.AddNode(bspNode);
			
			std::shared_ptr<CAnimatedQuadList> animatedQuadList = doomrpg->GetGeometryManager().AcquireGeometry<CAnimatedQuadList>(meshId, name + "_walls_floor_ceiling_leaf_node_" + std::to_string(nodeIndex));

			std::shared_ptr<CMaterial> material = doomrpg->GetMaterialManager().AcquireMaterial(1, "texture");
			material->SetTexture(doomrpg->GetTextures());

			std::shared_ptr<CAnimatedMesh> animatedMesh = doomrpg->GetMeshManager().AcquireMesh<CAnimatedMesh>(meshId--, name + "_walls_floor_ceiling_leaf_node_" + std::to_string(nodeIndex));
			animatedMesh->Initialize("common", animatedQuadList, std::move(material));
			m_bspTree.AddSceneEntity(nodeIndex, CSceneEntity(m_game, animatedMesh));
			wallFloorCeilingLookup.insert(std::make_pair(nodeIndex, animatedQuadList));

			for (unsigned int j = 0; j < node->argument2; j++)
			{
				const linesegment_t *line = (doomrpg->IsEx() ? (linesegment_t *)&m_mapex->lines[node->argument1 + j] : &m_map->lines[node->argument1 + j]);

				if ((line->flags & LF_DOOR) == LF_DOOR || line->flags & LF_SHIFT_EAST_OR_SOUTH || line->flags & LF_SHIFT_WEST_OR_NORTH)
					lines.push_back(std::make_pair(line, node->argument1 + j));
				else
				{
					float x1 = line->start.x * 8 / 64.0f;
					float z1 = line->start.y * 8 / 64.0f;
					float x2 = line->end.x * 8 / 64.0f;
					float z2 = line->end.y * 8 / 64.0f;
					float u = glm::abs(line->start.x == line->end.x ? line->end.y - line->start.y : line->end.x - line->start.x) * 8 / 64.0f;
					unsigned int textureIndex = mappings->wallMappings[line->texture];

					AnimatedQuad animatedQuad;
					animatedQuad.vertices[0] = { x1, 0.5f, z1, (line->flags & LF_FLIP_TEXTURE_HORIZONTALLY ? u : 0.0f), 0.0f };
					animatedQuad.vertices[1] = { x1, -0.5f, z1, (line->flags & LF_FLIP_TEXTURE_HORIZONTALLY ? u : 0.0f), 1.0f };
					animatedQuad.vertices[2] = { x2, 0.5f, z2, (line->flags & LF_FLIP_TEXTURE_HORIZONTALLY ? 0.0f : u), 0.0f };
					animatedQuad.vertices[3] = { x2, -0.5f, z2, (line->flags & LF_FLIP_TEXTURE_HORIZONTALLY ? 0.0f : u), 1.0f };
					animatedQuad.textureIndex = textureIndex;

					animatedQuadList->Add(animatedQuad);

					if (line->texture == 34 || line->texture == 54 || line->texture == 55)
					{
						unsigned int id = ENTITY_DOOR + line->texture;

						CEntity *entity = new CEntity(doomrpg, id);
						entity->SetHidden(false);
						entity->SetX(x1 + (x2 - x1) / 2);
						entity->SetZ(z1 + (z2 - z1) / 2);
						entity->UpdateTransformationMatrix();

						m_entityLocations.insert(std::make_pair((static_cast<unsigned int>(entity->GetZ() - (line->flags & LF_NORTH_FACING_WALL ? 1.0f : 0.0f)) << 8) | static_cast<unsigned int>(entity->GetX() - (line->flags & LF_WEST_FACING_WALL ? 1.0f : 0.0f)), m_entities.size()));
						m_entities.push_back(entity);
					}
				}
			}
		}
		else if (node->nodeType == 1)
		{
			bspNode.type = ECBSPNodeType::VerticalSplit;
			bspNode.bboxMin = glm::vec3(node->boundingBoxMin.x * 8 / 64.0f, -0.5f, node->boundingBoxMin.y * 8 / 64.0f);
			bspNode.bboxMax = glm::vec3(node->boundingBoxMax.x * 8 / 64.0f, 0.5f, node->boundingBoxMax.y * 8 / 64.0f);
			bspNode.splitPosition = node->splitPosition * 8 / 64.0f;
			bspNode.left = node->argument2;
			bspNode.right = node->argument1;

			m_bspTree.AddNode(bspNode);
		}
		else if (node->nodeType == 2)
		{
			bspNode.type = ECBSPNodeType::HorizontalSplit;
			bspNode.bboxMin = glm::vec3(node->boundingBoxMin.x * 8 / 64.0f, -0.5f, node->boundingBoxMin.y * 8 / 64.0f);
			bspNode.bboxMax = glm::vec3(node->boundingBoxMax.x * 8 / 64.0f, 0.5f, node->boundingBoxMax.y * 8 / 64.0f);
			bspNode.splitPosition = node->splitPosition * 8 / 64.0f;
			bspNode.left = node->argument2;
			bspNode.right = node->argument1;

			m_bspTree.AddNode(bspNode);
		}
	}

	for (unsigned int i = 0; i < lines.size(); i++)
	{
		const linesegment_t *line = lines[i].first;
		unsigned int index = lines[i].second;

		CDoor *door = new CDoor(doomrpg, line);
		m_doors.insert(std::make_pair(index, m_entities.size()));
		m_entityLocations.insert(std::make_pair((static_cast<unsigned int>(door->GetZ()) << 8) | static_cast<unsigned int>(door->GetX()), m_entities.size()));
		m_entities.push_back(door);
	}

	for (unsigned int i = 0; i < thingCount; i++)
	{
		const thing_t *thing = &things[i];

		unsigned int nodeIndex = m_bspTree.FindLeafNode(thing->position.x * 8 / 64.0f, thing->position.y * 8 / 64.0f);

		if (thing->flags & TF_WALL_DECORATION || (thing->flags & TF_FENCE) == TF_FENCE)
		{
			float x1, z1, x2, z2;

			if (thing->flags & TF_NORTH_FACING)
			{
				x1 = (thing->position.x * 8 + 32) / 64.0f;
				z1 = thing->position.y * 8 / 64.0f;
				x2 = (thing->position.x * 8 - 32) / 64.0f;
				z2 = thing->position.y * 8 / 64.0f;

				if (thing->flags & TF_WALL_DECORATION)
				{
					z1 -= 0.001f;
					z2 -= 0.001f;
				}
			}
			else if (thing->flags & TF_SOUTH_FACING)
			{
				x1 = (thing->position.x * 8 - 32) / 64.0f;
				z1 = thing->position.y * 8 / 64.0f;
				x2 = (thing->position.x * 8 + 32) / 64.0f;
				z2 = thing->position.y * 8 / 64.0f;

				if (thing->flags & TF_WALL_DECORATION)
				{
					z1 += 0.001f;
					z2 += 0.001f;
				}
			}
			else if (thing->flags & TF_EAST_FACING)
			{
				x1 = thing->position.x * 8 / 64.0f;
				z1 = (thing->position.y * 8 + 32) / 64.0f;
				x2 = thing->position.x * 8 / 64.0f;
				z2 = (thing->position.y * 8 - 32) / 64.0f;

				if (thing->flags & TF_WALL_DECORATION)
				{
					x1 += 0.001f;
					x2 += 0.001f;
				}
			}
			else if (thing->flags & TF_WEST_FACING)
			{
				x1 = thing->position.x * 8 / 64.0f;
				z1 = (thing->position.y * 8 - 32) / 64.0f;
				x2 = thing->position.x * 8 / 64.0f;
				z2 = (thing->position.y * 8 + 32) / 64.0f;

				if (thing->flags & TF_WALL_DECORATION)
				{
					x1 -= 0.001f;
					x2 -= 0.001f;
				}
			}

			unsigned int textureIndex = mappings->thingMappings[thing->id];

			if (thing->flags & TF_WALL_DECORATION)
			{
				if (decorationLookup.count(nodeIndex) == 0)
				{
					std::shared_ptr<CAnimatedQuadList> animatedQuadList = doomrpg->GetGeometryManager().AcquireGeometry<CAnimatedQuadList>(meshId, name + "_decorations_leaf_node_" + std::to_string(nodeIndex));

					std::shared_ptr<CMaterial> material = doomrpg->GetMaterialManager().AcquireMaterial(2, "decoration");
					material->SetTexture(doomrpg->GetSprites());

					std::shared_ptr<CAnimatedMesh> animatedMesh = doomrpg->GetMeshManager().AcquireMesh<CAnimatedMesh>(meshId--, name + "_decorations_leaf_node_" + std::to_string(nodeIndex));
					animatedMesh->Initialize("common", animatedQuadList, std::move(material));
					m_bspTree.AddSceneEntity(nodeIndex, CSceneEntity(m_game, animatedMesh));
					decorationLookup.insert(std::make_pair(nodeIndex, animatedQuadList));
				}

				AnimatedQuad animatedQuad;
				animatedQuad.vertices[0] = { x1, 0.5f, z1, 0.0f, 0.0f };
				animatedQuad.vertices[1] = { x1, -0.5f, z1, 0.0f, 1.0f };
				animatedQuad.vertices[2] = { x2, 0.5f, z2, 1.0f, 0.0f };
				animatedQuad.vertices[3] = { x2, -0.5f, z2, 1.0f, 1.0f };
				animatedQuad.textureIndex = textureIndex;

				decorationLookup[nodeIndex]->Add(animatedQuad);
			}
			else if (thing->id == ENTITY_LIGHT_2)
			{
				if (lightLookup.count(nodeIndex) == 0)
				{
					std::shared_ptr<CAnimatedQuadList> animatedQuadList = doomrpg->GetGeometryManager().AcquireGeometry<CAnimatedQuadList>(meshId, name + "_lights_leaf_node_" + std::to_string(nodeIndex));

					std::shared_ptr<CMaterial> material = doomrpg->GetMaterialManager().AcquireMaterial(3, "light");
					material->SetTexture(doomrpg->GetSprites());
					material->SetProperties(ECMaterialProperties::DoubleSided | ECMaterialProperties::Transparent);
					material->SetBlendMode(ECBlendMode::Additive);

					std::shared_ptr<CAnimatedMesh> animatedMesh = doomrpg->GetMeshManager().AcquireMesh<CAnimatedMesh>(meshId--, name + "_lights_leaf_node_" + std::to_string(nodeIndex));
					animatedMesh->Initialize("common", animatedQuadList, std::move(material));
					m_bspTree.AddSceneEntity(nodeIndex, CSceneEntity(m_game, animatedMesh));
					lightLookup.insert(std::make_pair(nodeIndex, animatedQuadList));
				}

				AnimatedQuad animatedQuad;
				animatedQuad.vertices[0] = { x1, 0.5f, z1, 0.0f, 0.0f };
				animatedQuad.vertices[1] = { x1, -0.5f, z1, 0.0f, 1.0f };
				animatedQuad.vertices[2] = { x2, 0.5f, z2, 1.0f, 0.0f };
				animatedQuad.vertices[3] = { x2, -0.5f, z2, 1.0f, 1.0f };
				animatedQuad.textureIndex = textureIndex;

				lightLookup[nodeIndex]->Add(animatedQuad);
			}
			else
			{
				if (fenceLookup.count(nodeIndex) == 0)
				{
					std::shared_ptr<CAnimatedQuadList> animatedQuadList = doomrpg->GetGeometryManager().AcquireGeometry<CAnimatedQuadList>(meshId, name + "_fences_leaf_node_" + std::to_string(nodeIndex));

					std::shared_ptr<CMaterial> material = doomrpg->GetMaterialManager().AcquireMaterial(4, "fence");
					material->SetTexture(doomrpg->GetSprites());
					material->SetProperties(ECMaterialProperties::DoubleSided);

					std::shared_ptr<CAnimatedMesh> animatedMesh = doomrpg->GetMeshManager().AcquireMesh<CAnimatedMesh>(meshId--, name + "_fences_leaf_node_" + std::to_string(nodeIndex));
					animatedMesh->Initialize("common", animatedQuadList, std::move(material));
					m_bspTree.AddSceneEntity(nodeIndex, CSceneEntity(m_game, animatedMesh));
					fenceLookup.insert(std::make_pair(nodeIndex, animatedQuadList));
				}

				AnimatedQuad animatedQuad;
				animatedQuad.vertices[0] = { x1, 0.5f, z1, 0.0f, 0.0f };
				animatedQuad.vertices[1] = { x1, -0.5f, z1, 0.0f, 1.0f };
				animatedQuad.vertices[2] = { x2, 0.5f, z2, 1.0f, 0.0f };
				animatedQuad.vertices[3] = { x2, -0.5f, z2, 1.0f, 1.0f };
				animatedQuad.textureIndex = textureIndex;

				fenceLookup[nodeIndex]->Add(animatedQuad);

				CThing *newThing = new CThing(doomrpg, thing);

				if (thing->flags & TF_NORTH_FACING || thing->flags & TF_SOUTH_FACING)
				{
					m_entityLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ() - 0.5f) << 8) | static_cast<unsigned int>(newThing->GetX()), m_entities.size()));
					m_entityLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ() + 0.5f) << 8) | static_cast<unsigned int>(newThing->GetX()), m_entities.size()));
					m_thingLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ() - 0.5f) << 8) | static_cast<unsigned int>(newThing->GetX()), m_things.size()));
					m_thingLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ() + 0.5f) << 8) | static_cast<unsigned int>(newThing->GetX()), m_things.size()));
				}
				else
				{
					m_entityLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ()) << 8) | static_cast<unsigned int>(newThing->GetX() - 0.5f), m_entities.size()));
					m_entityLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ()) << 8) | static_cast<unsigned int>(newThing->GetX() + 0.5f), m_entities.size()));
					m_thingLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ()) << 8) | static_cast<unsigned int>(newThing->GetX() - 0.5f), m_things.size()));
					m_thingLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ()) << 8) | static_cast<unsigned int>(newThing->GetX() + 0.5f), m_things.size()));
				}

				m_entities.push_back(newThing);
				m_things.push_back(newThing);
			}
		}
		else
		{
			const entity_t *entity = doomrpg->GetEntity(thing->id);

			CThing *newThing = nullptr;

			if (entity != nullptr)
			{
				if (entity->type == ENTITY_TYPE_PICKUP)
				{
					if (entity->parameter1 == ENTITY_PICKUP_HEALTH)
						newThing = new CHealth(doomrpg, thing);
					else if (entity->parameter1 == ENTITY_PICKUP_ARMOR)
						newThing = new CArmor(doomrpg, thing);
					else if (entity->parameter1 == ENTITY_PICKUP_CREDITS_1 || entity->parameter1 == ENTITY_PICKUP_CREDITS_2)
						newThing = new CCredit(doomrpg, thing);
					else if (entity->parameter1 == ENTITY_PICKUP_KEYS)
						newThing = new CKey(doomrpg, thing);
				}
				else if (entity->type == ENTITY_TYPE_CONSUMABLE)
				{
					if (entity->parameter1 == ENTITY_CONSUMABLE_SM_MEDKIT || entity->parameter1 == ENTITY_CONSUMABLE_LG_MEDKIT)
						newThing = new CMedkit(doomrpg, thing);
					else if (entity->parameter1 == ENTITY_CONSUMABLE_SOUL_SPHERE)
						newThing = new CSoulSphere(doomrpg, thing);
					else if (entity->parameter1 == ENTITY_CONSUMABLE_BERSERKER)
						newThing = new CBerserker(doomrpg, thing);
					else if (entity->parameter1 == ENTITY_CONSUMABLE_DOG_COLLAR)
						newThing = new CDogCollar(doomrpg, thing);
				}
				else if (entity->type == ENTITY_TYPE_WEAPON)
					newThing = new CWeapon(doomrpg, thing);
				else if (entity->type == ENTITY_TYPE_AMMO_SINGLE || entity->type == ENTITY_TYPE_AMMO_MULTIPLE)
					newThing = new CAmmo(doomrpg, thing);
				else
					newThing = new CThing(doomrpg, thing);
			}
			else
				newThing = new CThing(doomrpg, thing);

			m_entityLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ()) << 8) | static_cast<unsigned int>(newThing->GetX()), m_entities.size()));
			m_thingLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ()) << 8) | static_cast<unsigned int>(newThing->GetX()), m_things.size()));
			m_entities.push_back(newThing);
			m_things.push_back(newThing);

			if (doomrpg->IsEx() && thing->id == ENTITY_YELLOW_LAMP)
			{
				thing_t light = *thing;
				light.id = ENTITY_LIGHT_1;

				newThing = new CThing(doomrpg, &light);

				m_entityLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ()) << 8) | static_cast<unsigned int>(newThing->GetX()), m_entities.size()));
				m_thingLocations.insert(std::make_pair((static_cast<unsigned int>(newThing->GetZ()) << 8) | static_cast<unsigned int>(newThing->GetX()), m_things.size()));
				m_entities.push_back(newThing);
				m_things.push_back(newThing);
			}
		}
	}

	if (doomrpg->IsEx())
	{
		AnimatedQuad animatedQuad;

		for (unsigned int y = 0; y < 32; y++)
		{
			for (unsigned int x = 0; x < 32; x++)
			{
				unsigned int nodeIndex = m_bspTree.FindLeafNode(x + 0.5f, y + 0.5f);

				animatedQuad.vertices[0] = { static_cast<float>(x), -0.5f, static_cast<float>(y), 1.0f, 0.0f };
				animatedQuad.vertices[1] = { static_cast<float>(x), -0.5f, static_cast<float>(y + 1), 0.0f, 0.0f };
				animatedQuad.vertices[2] = { static_cast<float>(x + 1), -0.5f, static_cast<float>(y), 1.0f, 1.0f };
				animatedQuad.vertices[3] = { static_cast<float>(x + 1), -0.5f, static_cast<float>(y + 1), 0.0f, 1.0f };
				animatedQuad.textureIndex = mappings->wallMappings[m_mapex->floorMap[y * 32 + x]];

				wallFloorCeilingLookup[nodeIndex]->Add(animatedQuad);

				animatedQuad.vertices[0] = { static_cast<float>(x + 1), 0.5f, static_cast<float>(y), 1.0f, 0.0f };
				animatedQuad.vertices[1] = { static_cast<float>(x + 1), 0.5f, static_cast<float>(y + 1), 0.0f, 0.0f };
				animatedQuad.vertices[2] = { static_cast<float>(x), 0.5f, static_cast<float>(y), 1.0f, 1.0f };
				animatedQuad.vertices[3] = { static_cast<float>(x), 0.5f, static_cast<float>(y + 1), 0.0f, 1.0f };
				animatedQuad.textureIndex = mappings->wallMappings[m_mapex->ceilingMap[y * 32 + x]];

				wallFloorCeilingLookup[nodeIndex]->Add(animatedQuad);
			}
		}
	}
	else
	{
		uint32_t *pixels = new uint32_t[64 * 128];

		uint32_t *pixel = &pixels[0];
		uint32_t color = (255 << 24) | (m_map->header.floorColor.b << 16) | (m_map->header.floorColor.g << 8) | m_map->header.floorColor.r;
		size_t count = 64 * 64;

		while (count--)
			*pixel++ = color;

		color = (255 << 24) | (m_map->header.ceilingColor.b << 16) | (m_map->header.ceilingColor.g << 8) | m_map->header.ceilingColor.r;
		count = 64 * 64;

		while (count--)
			*pixel++ = color;

		std::shared_ptr<CTexture> texture = std::shared_ptr<CTexture>(std::make_shared<CTexture>(m_game, pixels, 64, 128, 1, 2));

		delete[] pixels;

		/*std::vector<AnimatedSceneEntityVertexData> vertices;

		vertices.push_back({ m_nodes[0].bboxMin.x, m_nodes[0].bboxMin.y, m_nodes[0].bboxMin.z, m_nodes[0].bboxMin.x, m_nodes[0].bboxMin.z, 0.0f });
		vertices.push_back({ m_nodes[0].bboxMin.x, m_nodes[0].bboxMin.y, m_nodes[0].bboxMax.z, m_nodes[0].bboxMin.x, m_nodes[0].bboxMax.z, 0.0f });
		vertices.push_back({ m_nodes[0].bboxMax.x, m_nodes[0].bboxMin.y, m_nodes[0].bboxMin.z, m_nodes[0].bboxMax.x, m_nodes[0].bboxMin.z, 0.0f });
		vertices.push_back({ m_nodes[0].bboxMax.x, m_nodes[0].bboxMin.y, m_nodes[0].bboxMax.z, m_nodes[0].bboxMax.x, m_nodes[0].bboxMax.z, 0.0f });

		vertices.push_back({ m_nodes[0].bboxMax.x, m_nodes[0].bboxMax.y, m_nodes[0].bboxMin.z, m_nodes[0].bboxMax.x, m_nodes[0].bboxMin.z, 1.0f });
		vertices.push_back({ m_nodes[0].bboxMax.x, m_nodes[0].bboxMax.y, m_nodes[0].bboxMax.z, m_nodes[0].bboxMax.x, m_nodes[0].bboxMax.z, 1.0f });
		vertices.push_back({ m_nodes[0].bboxMin.x, m_nodes[0].bboxMax.y, m_nodes[0].bboxMin.z, m_nodes[0].bboxMin.x, m_nodes[0].bboxMin.z, 1.0f });
		vertices.push_back({ m_nodes[0].bboxMin.x, m_nodes[0].bboxMax.y, m_nodes[0].bboxMax.z, m_nodes[0].bboxMin.x, m_nodes[0].bboxMax.z, 1.0f });

		m_staticGeometry.push_back(CAnimatedSceneEntity(m_game, vertices, texture));*/
	}
}

const std::string CMap::m_resourcePath = "maps/";