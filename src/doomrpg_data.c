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

#include <stdio.h>
#include <stdlib.h>

#include "doomrpg_data.h"

mappings_t *LoadMappings(const char *filename)
{
	mappings_t *mappings = NULL;
	FILE *fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		mappings = (mappings_t *)malloc(sizeof(mappings_t));
		fread(&mappings->textureMappingCount, sizeof(uint32_t), 1, fp);
		fread(&mappings->spriteMappingCount, sizeof(uint32_t), 1, fp);
		fread(&mappings->wallMappingCount, sizeof(uint32_t), 1, fp);
		fread(&mappings->thingMappingCount, sizeof(uint32_t), 1, fp);
		mappings->textureMappings = (texturemapping_t *)malloc(sizeof(texturemapping_t) * mappings->textureMappingCount);
		fread(mappings->textureMappings, sizeof(texturemapping_t), mappings->textureMappingCount, fp);
		mappings->spriteMappings = (spritemapping_t *)malloc(sizeof(spritemapping_t) * mappings->spriteMappingCount);
		fread(mappings->spriteMappings, sizeof(spritemapping_t), mappings->spriteMappingCount, fp);
		mappings->wallMappings = (uint16_t *)malloc(sizeof(uint16_t) * mappings->wallMappingCount);
		fread(mappings->wallMappings, sizeof(uint16_t), mappings->wallMappingCount, fp);
		mappings->thingMappings = (uint16_t *)malloc(sizeof(uint16_t) * mappings->thingMappingCount);
		fread(mappings->thingMappings, sizeof(uint16_t), mappings->thingMappingCount, fp);
		fclose(fp);
	}
	return mappings;
}

uint8_t *LoadBitShapes(const char *filename)
{
	uint8_t *bitShapes = NULL;
	uint32_t length;
	FILE *fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		fread(&length, sizeof(uint32_t), 1, fp);
		bitShapes = (uint8_t *)malloc(length);
		fread(bitShapes, sizeof(uint8_t), length, fp);
		fclose(fp);
	}
	return bitShapes;
}

uint8_t *LoadTexels(const char *filename)
{
	uint8_t *texels = NULL;
	uint32_t length;
	FILE *fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		fread(&length, sizeof(uint32_t), 1, fp);
		texels = (uint8_t *)malloc(length);
		fread(texels, sizeof(uint8_t), length, fp);
		fclose(fp);
	}
	return texels;
}

uint16_t *LoadPalettes(const char *filename)
{
	uint16_t *palettes = NULL;
	uint32_t length;
	FILE *fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		fread(&length, sizeof(uint32_t), 1, fp);
		palettes = (uint16_t *)malloc(length);
		fread(palettes, sizeof(uint8_t), length, fp);
		fclose(fp);
	}
	return palettes;
}

entities_t *LoadEntities(const char *filename)
{
	entities_t *entities = NULL;
	FILE *fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		entities = (entities_t *)malloc(sizeof(entities_t));
		fread(&entities->entityCount, sizeof(uint16_t), 1, fp);
		entities->entities = (entity_t *)malloc(sizeof(entity_t) * entities->entityCount);
		fread(entities->entities, sizeof(entity_t), entities->entityCount, fp);
		fclose(fp);
	}
	return entities;
}

entitiesex_t *LoadEntitiesEx(const char *filename)
{
	entitiesex_t *entities = NULL;
	FILE *fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		entities = (entitiesex_t *)malloc(sizeof(entitiesex_t));
		fread(&entities->entityCount, sizeof(uint16_t), 1, fp);
		entities->entities = (entityex_t *)malloc(sizeof(entityex_t) * entities->entityCount);
		fread(entities->entities, sizeof(entityex_t), entities->entityCount, fp);
		fclose(fp);
	}
	return entities;
}

strings_t *LoadStrings(const char *filename)
{
	strings_t *strings = NULL;
	long size;
	char *buffer;
	char *pBuffer;
	uint16_t length;
	int i;
	FILE *fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		strings = (strings_t *)malloc(sizeof(strings_t));
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(&strings->stringCount, sizeof(uint16_t), 1, fp);
		strings->strings = (char **)malloc(sizeof(char *) * strings->stringCount);
		buffer = (char *)malloc(size - sizeof(uint16_t) * (strings->stringCount + 1) + sizeof(char) * strings->stringCount);
		pBuffer = buffer;
		for (i = 0; i < strings->stringCount; i++)
		{
			fread(&length, sizeof(uint16_t), 1, fp);
			fread(pBuffer, sizeof(char), length, fp);
			strings->strings[i] = pBuffer;
			pBuffer += length;
			*pBuffer = '\0';
			pBuffer++;
		}
		fclose(fp);
	}
	return strings;
}

bspmap_t *LoadBspMap(const char *filename)
{
	bspmap_t *map = NULL;
	FILE *fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		map = (bspmap_t *)malloc(sizeof(bspmap_t));
		fread(&map->header, sizeof(bspheader_t), 1, fp);
		fread(&map->nodeCount, sizeof(uint16_t), 1, fp);
		map->nodes = (bspnode_t *)malloc(sizeof(bspnode_t) * map->nodeCount);
		fread(map->nodes, sizeof(bspnode_t), map->nodeCount, fp);
		fread(&map->lineCount, sizeof(uint16_t), 1, fp);
		map->lines = (linesegment_t *)malloc(sizeof(linesegment_t) * map->lineCount);
		fread(map->lines, sizeof(linesegment_t), map->lineCount, fp);
		fread(&map->thingCount, sizeof(uint16_t), 1, fp);
		map->things = (thing_t *)malloc(sizeof(thing_t) * map->thingCount);
		fread(map->things, sizeof(thing_t), map->thingCount, fp);
		fread(&map->eventCount, sizeof(uint16_t), 1, fp);
		map->events = (uint32_t *)malloc(sizeof(uint32_t) * map->eventCount);
		fread(map->events, sizeof(uint32_t), map->eventCount, fp);
		fread(&map->commandCount, sizeof(uint16_t), 1, fp);
		map->commands = (command_t *)malloc(sizeof(command_t) * map->commandCount);
		fread(map->commands, sizeof(command_t), map->commandCount, fp);
		fread(map->blockMap, sizeof(uint8_t), 256, fp);
		fclose(fp);
	}
	return map;
}

bspmapex_t *LoadBspMapEx(const char *filename)
{
	bspmapex_t *map = NULL;
	uint16_t stringCount;
	long offset;
	long size;
	char *buffer;
	char *pBuffer;
	uint16_t length;
	int i;
	FILE *fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		map = (bspmapex_t *)malloc(sizeof(bspmapex_t));
		fread(&map->header, sizeof(bspheaderex_t), 1, fp);
		fread(&map->nodeCount, sizeof(uint16_t), 1, fp);
		map->nodes = (bspnode_t *)malloc(sizeof(bspnode_t) * map->nodeCount);
		fread(map->nodes, sizeof(bspnode_t), map->nodeCount, fp);
		fread(&map->lineCount, sizeof(uint16_t), 1, fp);
		map->lines = (linesegmentex_t *)malloc(sizeof(linesegmentex_t) * map->lineCount);
		fread(map->lines, sizeof(linesegmentex_t), map->lineCount, fp);
		fread(&map->thingCount, sizeof(uint16_t), 1, fp);
		map->things = (thing_t *)malloc(sizeof(thing_t) * map->thingCount);
		fread(map->things, sizeof(thing_t), map->thingCount, fp);
		fread(&map->eventCount, sizeof(uint16_t), 1, fp);
		map->events = (uint32_t *)malloc(sizeof(uint32_t) * map->eventCount);
		fread(map->events, sizeof(uint32_t), map->eventCount, fp);
		fread(&map->commandCount, sizeof(uint16_t), 1, fp);
		map->commands = (command_t *)malloc(sizeof(command_t) * map->commandCount);
		fread(map->commands, sizeof(command_t), map->commandCount, fp);
		fread(&stringCount, sizeof(uint16_t), 1, fp);
		map->strings = NULL;
		if (stringCount != 0)
		{
			map->strings = (strings_t *)malloc(sizeof(strings_t));
			map->strings->stringCount = stringCount;
			offset = ftell(fp);
			fseek(fp, -2304, SEEK_END);
			size = ftell(fp) - offset;
			fseek(fp, offset, SEEK_SET);
			map->strings->strings = (char **)malloc(sizeof(char *) * map->strings->stringCount);
			buffer = (char *)malloc(size - sizeof(uint16_t) * map->strings->stringCount + sizeof(char) * map->strings->stringCount);
			pBuffer = buffer;
			for (i = 0; i < map->strings->stringCount; i++)
			{
				fread(&length, sizeof(uint16_t), 1, fp);
				fread(pBuffer, sizeof(char), length, fp);
				map->strings->strings[i] = pBuffer;
				pBuffer += length;
				*pBuffer = '\0';
				pBuffer++;
			}
		}
		fread(map->blockMap, sizeof(uint8_t), 256, fp);
		fread(map->floorMap, sizeof(uint8_t), 1024, fp);
		fread(map->ceilingMap, sizeof(uint8_t), 1024, fp);
		fclose(fp);
	}
	return map;
}

void FreeMappings(mappings_t *mappings)
{
	if (mappings != NULL)
	{
		free(mappings->textureMappings);
		free(mappings->spriteMappings);
		free(mappings->wallMappings);
		free(mappings->thingMappings);
		free(mappings);
	}
}

void FreeBitShapes(uint8_t *bitShapes)
{
	if (bitShapes != NULL)
		free(bitShapes);
}

void FreeTexels(uint8_t *texels)
{
	if (texels != NULL)
		free(texels);
}

void FreePalettes(uint16_t *palettes)
{
	if (palettes != NULL)
		free(palettes);
}

void FreeEntities(entities_t *entities)
{
	if (entities != NULL)
	{
		free(entities->entities);
		free(entities);
	}
}

void FreeEntitiesEx(entitiesex_t *entities)
{
	if (entities != NULL)
	{
		free(entities->entities);
		free(entities);
	}
}

void FreeStrings(strings_t *strings)
{
	if (strings != NULL)
	{
		free(*strings->strings);
		free(strings->strings);
		free(strings);
	}
}

void FreeBspMap(bspmap_t *bspmap)
{
	if (bspmap != NULL)
	{
		free(bspmap->nodes);
		free(bspmap->lines);
		free(bspmap->things);
		free(bspmap->events);
		free(bspmap->commands);
		free(bspmap);
	}
}

void FreeBspMapEx(bspmapex_t *bspmap)
{
	if (bspmap != NULL)
	{
		free(bspmap->nodes);
		free(bspmap->lines);
		free(bspmap->things);
		free(bspmap->events);
		free(bspmap->commands);
		FreeStrings(bspmap->strings);
		free(bspmap);
	}
}