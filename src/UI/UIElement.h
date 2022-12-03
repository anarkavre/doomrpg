#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__

#include <string>
#include <type_traits>

struct UIElementRect
{
	int x, y;
	int w, h;
};

struct UIElementTexture
{
	std::string name;
	UIElementRect rect;
};

enum class ECUIElementHorizontalAlignment
{
	None,
	Left,
	Right,
	Center
};

enum class ECUIElementVerticalAlignment
{
	None,
	Top,
	Bottom,
	Center
};

enum class ECUIElementProperties
{
	None						= 0x0,
	RepeatTextureHorizontally	= 0x1,
	RepeatTextureVertically		= 0x2,
	FillParentWidth				= 0x4,
	FillParentHeight			= 0x8
};

inline ECUIElementProperties operator |(ECUIElementProperties a, ECUIElementProperties b)
{
	return static_cast<ECUIElementProperties>(static_cast<std::underlying_type<ECUIElementProperties>::type>(a) | static_cast<std::underlying_type<ECUIElementProperties>::type>(b));
}

inline ECUIElementProperties operator &(ECUIElementProperties a, ECUIElementProperties b)
{
	return static_cast<ECUIElementProperties>(static_cast<std::underlying_type<ECUIElementProperties>::type>(a) & static_cast<std::underlying_type<ECUIElementProperties>::type>(b));
}

inline ECUIElementProperties operator ^(ECUIElementProperties a, ECUIElementProperties b)
{
	return static_cast<ECUIElementProperties>(static_cast<std::underlying_type<ECUIElementProperties>::type>(a) ^ static_cast<std::underlying_type<ECUIElementProperties>::type>(b));
}

inline ECUIElementProperties operator ~(ECUIElementProperties a)
{
	return static_cast<ECUIElementProperties>(~static_cast<std::underlying_type<ECUIElementProperties>::type>(a));
}

inline ECUIElementProperties &operator |=(ECUIElementProperties &a, ECUIElementProperties b)
{
	return reinterpret_cast<ECUIElementProperties &>(reinterpret_cast<std::underlying_type<ECUIElementProperties>::type &>(a) |= static_cast<std::underlying_type<ECUIElementProperties>::type>(b));
}

inline ECUIElementProperties &operator &=(ECUIElementProperties &a, ECUIElementProperties b)
{
	return reinterpret_cast<ECUIElementProperties &>(reinterpret_cast<std::underlying_type<ECUIElementProperties>::type &>(a) &= static_cast<std::underlying_type<ECUIElementProperties>::type>(b));
}

inline ECUIElementProperties &operator ^=(ECUIElementProperties &a, ECUIElementProperties b)
{
	return reinterpret_cast<ECUIElementProperties &>(reinterpret_cast<std::underlying_type<ECUIElementProperties>::type &>(a) ^= static_cast<std::underlying_type<ECUIElementProperties>::type>(b));
}

#include "CUIElement.h"
#include "CUIElementStateful.h"

#endif