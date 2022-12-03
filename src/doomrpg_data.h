// doomrpg_data
// Copyright(C) 2020-2022 John D. Corrado
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#ifndef __DOOMRPG_DATA_H__
#define __DOOMRPG_DATA_H__

#include <stdint.h>

#include "doomrpg_entities.h"

#define LF_DOOR							0x0005
#define LF_SHIFT_EAST_OR_SOUTH			0x0008
#define LF_SHIFT_WEST_OR_NORTH			0x0010
#define LF_SECRET_OR_HIDDEN				0x0020
#define LF_SHIFT_HORIZONTAL				0x0100
#define LF_SHIFT_VERTICAL				0x0200
#define LF_LOCKED_DOOR					0x0400
#define LF_NORTH_FACING_WALL			0x0800
#define LF_SOUTH_FACING_WALL			0x1000
#define LF_EAST_FACING_WALL				0x2000
#define LF_WEST_FACING_WALL				0x4000
#define LF_FLIP_TEXTURE_HORIZONTALLY	0x8000

#define TF_HIDDEN									0x0001
#define TF_FENCE									0x0802
#define TF_TREAT_AS_WALL							0x0004
#define TF_NORTH_FACING								0x0008
#define TF_SOUTH_FACING								0x0010
#define TF_EAST_FACING								0x0020
#define TF_WEST_FACING								0x0040
#define TF_WALL_DECORATION							0x0080
#define TF_DEAD_ENEMY								0x0100
#define TF_NORTH_OR_SOUTH_FACING_WALL_DECORATION	0x0200

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	uint32_t texture;
	uint32_t palette;
} texturemapping_t;

typedef struct
{
	uint32_t sprite;
	uint32_t palette;
} spritemapping_t;

typedef struct
{
	uint32_t textureMappingCount;
	texturemapping_t *textureMappings;
	uint32_t spriteMappingCount;
	spritemapping_t *spriteMappings;
	uint32_t wallMappingCount;
	uint16_t *wallMappings;
	uint32_t thingMappingCount;
	uint16_t *thingMappings;
} mappings_t;

typedef struct
{
	uint32_t spriteTexelsOffset;
	uint16_t byteCount;
	uint16_t bufferSize;
	uint8_t xOffsetMin;
	uint8_t xOffsetMax;
	uint8_t yOffsetMin;
	uint8_t yOffsetMax;
} bitshape_t;

typedef struct
{
	uint16_t id;
	uint8_t type;
	uint8_t parameter1;
	uint32_t parameter2;
} entity_t;

typedef struct
{
	uint16_t entityCount;
	entity_t *entities;
} entities_t;

typedef struct
{
	uint16_t id;
	uint8_t type;
	uint8_t parameter1;
	uint32_t parameter2;
	uint8_t name[16];
} entityex_t;

typedef struct
{
	uint16_t entityCount;
	entityex_t *entities;
} entitiesex_t;

typedef struct
{
	uint16_t stringCount;
	char **strings;
} strings_t;

#pragma pack(push, 1)

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t;

typedef struct
{
	uint8_t x;
	uint8_t y;
} coordinate_t;

typedef struct
{
	color_t floorColor;
	color_t ceilingColor;
	color_t introColor;
	uint8_t levelId;
	uint16_t playerPosition;
	uint8_t playerAngle;
} bspheader_t;

typedef struct
{
	uint8_t name[16];
	color_t floorColor;
	color_t ceilingColor;
	uint8_t floorTexture;
	uint8_t ceilingTexture;
	color_t introColor;
	uint8_t levelId;
	uint16_t playerPosition;
	uint8_t playerAngle;
	uint16_t cameraPosition;
} bspheaderex_t;

typedef struct
{
	coordinate_t boundingBoxMin;
	coordinate_t boundingBoxMax;
	uint8_t nodeType;
	uint8_t splitPosition;
	uint16_t argument1;
	uint16_t argument2;
} bspnode_t;

typedef struct
{
	coordinate_t start;
	coordinate_t end;
	uint16_t texture;
	uint16_t flags;
} linesegment_t;

typedef struct
{
	coordinate_t start;
	coordinate_t end;
	uint16_t texture;
	uint16_t flags;
	uint16_t unused;
} linesegmentex_t;

typedef struct
{
	coordinate_t position;
	uint8_t id;
	uint16_t flags;
} thing_t;

typedef struct
{
	uint8_t id;
	uint32_t argument1;
	uint32_t argument2;
} command_t;

#pragma pack(pop)

typedef struct
{
	bspheader_t header;
	uint16_t nodeCount;
	bspnode_t *nodes;
	uint16_t lineCount;
	linesegment_t *lines;
	uint16_t thingCount;
	thing_t *things;
	uint16_t eventCount;
	uint32_t *events;
	uint16_t commandCount;
	command_t *commands;
	uint8_t blockMap[256];
} bspmap_t;

typedef struct
{
	bspheaderex_t header;
	uint16_t nodeCount;
	bspnode_t *nodes;
	uint16_t lineCount;
	linesegmentex_t *lines;
	uint16_t thingCount;
	thing_t *things;
	uint16_t eventCount;
	uint32_t *events;
	uint16_t commandCount;
	command_t *commands;
	strings_t *strings;
	uint8_t blockMap[256];
	uint8_t floorMap[1024];
	uint8_t ceilingMap[1024];
} bspmapex_t;

mappings_t *LoadMappings(const char *filename);
uint8_t *LoadBitShapes(const char *filename);
uint8_t *LoadTexels(const char *filename);
uint16_t *LoadPalettes(const char *filename);
entities_t *LoadEntities(const char *filename);
entitiesex_t *LoadEntitiesEx(const char *filename);
strings_t *LoadStrings(const char *filename);
bspmap_t *LoadBspMap(const char *filename);
bspmapex_t *LoadBspMapEx(const char *filename);
void FreeMappings(mappings_t *mappings);
void FreeBitShapes(uint8_t *bitShapes);
void FreeTexels(uint8_t *texels);
void FreePalettes(uint16_t *palettes);
void FreeEntities(entities_t *entities);
void FreeEntitiesEx(entitiesex_t *entities);
void FreeStrings(strings_t *strings);
void FreeBspMap(bspmap_t *bspmap);
void FreeBspMapEx(bspmapex_t *bspmap);

#ifdef __cplusplus
}
#endif

#endif