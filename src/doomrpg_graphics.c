// doomrpg_graphics
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "doomrpg_graphics.h"

uint32_t *CreateRGBAImageFromWallTextures(const texturemapping_t *textureMappings, uint32_t textureMappingCount, const uint8_t *wallTexels, const uint16_t *palettes, uint32_t *imageWidth, uint32_t *imageHeight)
{
	uint32_t i = 6;
	uint32_t imageDimension = 64;

	while (imageDimension * imageDimension / 4096 < textureMappingCount)
		imageDimension = 1 << (i++);

	uint32_t tileDimension = imageDimension / 64;

	uint32_t *pixels = (uint32_t *)malloc(imageDimension * imageDimension * sizeof(uint32_t));

	memset(pixels, 0, imageDimension * imageDimension * sizeof(uint32_t));

	const texturemapping_t *textureMapping = textureMappings;
	uint32_t *pColumn = pixels - 1;
	uint32_t *pRow = NULL;

	for (uint32_t j = 1; j <= textureMappingCount; j++)
	{
		const uint8_t *wallTexel = wallTexels + textureMapping->texture / 2;
		const uint16_t *palette = palettes + textureMapping->palette;
		textureMapping++;

		for (uint32_t i = 0; i < 2048; i++)
		{
			if (!(i & 31))
			{
				pColumn++;
				pRow = pColumn;
			}

			uint32_t index = *wallTexel & 0x0F;
			uint16_t color = palette[index];
			uint8_t r = color & 0x1F;
			uint8_t g = (color >> 5) & 0x3F;
			uint8_t b = (color >> 11) & 0x1F;
			r = (r << 3) | (r >> 2);
			g = (g << 2) | (g >> 4);
			b = (b << 3) | (b >> 2);
			*pRow = (255 << 24) | (b << 16) | (g << 8) | r;
			pRow += imageDimension;
			index = (*wallTexel >> 4) & 0x0F;
			color = palette[index];
			r = color & 0x1F;
			g = (color >> 5) & 0x3F;
			b = (color >> 11) & 0x1F;
			r = (r << 3) | (r >> 2);
			g = (g << 2) | (g >> 4);
			b = (b << 3) | (b >> 2);
			*pRow = (255 << 24) | (b << 16) | (g << 8) | r;
			pRow += imageDimension;
			wallTexel++;
		}

		if (!(j & (tileDimension - 1)))
		{
			pColumn = pRow - imageDimension;
		}
	}

	if (imageWidth)
		*imageWidth = imageDimension;

	if (imageHeight)
		*imageHeight = imageDimension;

	return pixels;
}

uint32_t *CreateRGBAImageFromSprites(const spritemapping_t *spriteMappings, uint32_t spriteMappingCount, const uint8_t *bitShapes, const uint8_t *spriteTexels, const uint16_t *palettes, uint32_t *imageWidth, uint32_t *imageHeight)
{
	uint32_t i = 6;
	uint32_t imageDimension = 64;

	while (imageDimension * imageDimension / 4096 < spriteMappingCount)
		imageDimension = 1 << (i++);

	uint32_t tileDimension = imageDimension / 64;

	uint32_t *pixels = (uint32_t *)malloc(imageDimension * imageDimension * sizeof(uint32_t));

	memset(pixels, 0, imageDimension * imageDimension * sizeof(uint32_t));

	const spritemapping_t *spriteMapping = spriteMappings;
	uint32_t *pBase = pixels;
	uint32_t *pColumn;
	uint32_t *pRow = NULL;
	uint32_t startOffset = 0;

	for (uint32_t j = 1; j <= spriteMappingCount; j++)
	{
		const bitshape_t *bitShape = (const bitshape_t *)(bitShapes + spriteMapping->sprite);
		uint32_t byteCount = bitShape->byteCount - 4;
		uint32_t columnByteCount = (bitShape->yOffsetMax - bitShape->yOffsetMin + 8) / 8;
		const uint8_t *byte = bitShapes + spriteMapping->sprite + sizeof(bitshape_t);

		if (!startOffset)
			startOffset = bitShape->spriteTexelsOffset;

		const uint8_t *spriteTexel = spriteTexels + (bitShape->spriteTexelsOffset - startOffset) / 2;
		bool highNibble = false;
		const uint16_t *palette = palettes + spriteMapping->palette;
		spriteMapping++;

		pColumn = pBase + bitShape->yOffsetMin * imageDimension + bitShape->xOffsetMin - 1;

		for (uint32_t i = 0; i < byteCount; i++)
		{
			if (!(i % columnByteCount))
			{
				pColumn++;
				pRow = pColumn;
			}

			for (uint32_t bit = 0; bit < 8; bit++)
			{
				if (*byte & (1 << bit))
				{
					uint32_t index = (highNibble ? (*spriteTexel >> 4) & 0x0F : *spriteTexel & 0x0F);
					uint16_t color = palette[index];
					uint8_t r = color & 0x1F;
					uint8_t g = (color >> 5) & 0x3F;
					uint8_t b = (color >> 11) & 0x1F;
					r = (r << 3) | (r >> 2);
					g = (g << 2) | (g >> 4);
					b = (b << 3) | (b >> 2);
					*pRow = (255 << 24) | (b << 16) | (g << 8) | r;

					if (highNibble)
						spriteTexel++;

					highNibble = !highNibble;
				}

				pRow += imageDimension;
			}

			byte++;
		}

		if (!(j & (tileDimension - 1)))
		{
			pBase += 63 * imageDimension + 64;
		}
		else
		{
			pBase += 64;
		}
	}

	if (imageWidth)
		*imageWidth = imageDimension;

	if (imageHeight)
		*imageHeight = imageDimension;

	return pixels;
}