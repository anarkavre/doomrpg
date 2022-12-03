#ifndef __FONT_H__
#define __FONT_H__

#include <string>

struct Font
{
	std::string texture;
	unsigned int cellWidth, cellHeight;
	char baseCharacter;
	unsigned char characterWidth;
};

#endif