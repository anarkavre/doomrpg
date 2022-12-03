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

#ifndef __DOOMRPG_GRAPHICS_H__
#define __DOOMRPG_GRAPHICS_H__

#include "doomrpg_data.h"

#ifdef __cplusplus
extern "C"
{
#endif

uint32_t *CreateRGBAImageFromWallTextures(const texturemapping_t *textureMappings, uint32_t textureMappingCount, const uint8_t *wallTexels, const uint16_t *palettes, uint32_t *imageWidth, uint32_t *imageHeight);
uint32_t *CreateRGBAImageFromSprites(const spritemapping_t *spriteMappings, uint32_t spriteMappingCount, const uint8_t *bitShapes, const uint8_t *spriteTexels, const uint16_t *palettes, uint32_t *imageWidth, uint32_t *imageHeight);

#ifdef __cplusplus
}
#endif

#endif