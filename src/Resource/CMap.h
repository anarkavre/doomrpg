#ifndef __CMAP_H__
#define __CMAP_H__

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "glm/glm.hpp"

#include "CResource.h"
#include "Entity/Thing/CThing.h"
#include "Scene/CBSPTree.h"
#include "doomrpg_data.h"

struct Event
{
	unsigned int commandStart, commandCount;
	unsigned int currentCommand;
	unsigned int variable;
	bool halted;
	std::unordered_map<unsigned int, std::vector<unsigned int>> onActionCommands;
	std::unordered_map<unsigned int, std::vector<unsigned int>[4]> onEnterCommands;
	std::unordered_map<unsigned int, std::vector<unsigned int>[4]> onLeaveCommands;
	std::vector<unsigned int> onLookCommands[4];
};

class CAnimatedQuadList;
class CThing;

class CMap : public CResource
{
public:
	CMap(CGame *game, unsigned int handle, const std::string &name);
	~CMap();

	void ExecuteCommand(Event *event, unsigned int command);

	const glm::vec3 &GetPlayerPosition() const { return m_playerPosition; }
	float GetPlayerAngle() const { return m_playerAngle; }
	unsigned char GetBlock(unsigned int x, unsigned int y) const { return m_blockMap[y][x]; }
	CEntity *GetEntity(unsigned int entityIndex) const { return m_entities[entityIndex]; }
	void GetEntities(unsigned int x, unsigned int y, std::vector<unsigned int> &entityIndices) { auto range = m_entityLocations.equal_range((y << 8) | x); for_each(range.first, range.second, [&entityIndices](std::unordered_multimap<unsigned int, unsigned int>::value_type &value) { entityIndices.push_back(value.second); }); }
	CThing *GetThing(unsigned int thingIndex) const { return m_things[thingIndex]; }
	void GetThings(unsigned int x, unsigned int y, std::vector<unsigned int> &thingIndices) { auto range = m_thingLocations.equal_range((y << 8) | x); for_each(range.first, range.second, [&thingIndices](std::unordered_multimap<unsigned int, unsigned int>::value_type &value) { thingIndices.push_back(value.second); }); }
	Event *GetEvent(unsigned int x, unsigned int y) { return (m_events.count((y << 8) | x) == 1 ? &m_events.at((y << 8) | x) : nullptr); }

	static const std::string &GetResourcePath() { return m_resourcePath; }

private:
	void BuildMap();

	CBSPTree m_bspTree;
	std::unordered_map<unsigned int, unsigned int> m_doors;
	std::unordered_multimap<unsigned int, unsigned int> m_entityLocations;
	std::unordered_multimap<unsigned int, unsigned int> m_thingLocations;
	std::vector<CEntity *> m_entities;
	std::vector<CThing *> m_things;
	glm::vec3 m_playerPosition;
	float m_playerAngle;
	unsigned char m_blockMap[32][32];
	std::unordered_map<unsigned int, Event> m_events;
	bspmap_t *m_map;
	bspmapex_t *m_mapex;
	strings_t *m_strings;

	static const std::string m_resourcePath;
};

#endif