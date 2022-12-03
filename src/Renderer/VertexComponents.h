#ifndef __VERTEXCOMPONENTS_H__
#define __VERTEXCOMPONENTS_H__

enum class ECVertexComponent
{
	Position2D,
	Position3D,
	TexCoord2D,
	TexCoord3D,
	Normal,
	Color
};

namespace VertexComponent
{
	struct Position2D
	{
		float x, y;
	};

	struct Position3D
	{
		float x, y, z;
	};

	struct TexCoord2D
	{
		float u, v;
	};

	struct TexCoord3D
	{
		float u, v, w;
	};

	struct Normal
	{
		float x, y, z;
	};

	struct Color
	{
		float r, g, b, a;
	};
}

#endif