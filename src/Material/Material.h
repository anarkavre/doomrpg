#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <memory>
#include <type_traits>

enum class ECMaterialProperties
{
	None		= 0x0,
	DoubleSided	= 0x1,
	Transparent = 0x2
};

inline ECMaterialProperties operator |(ECMaterialProperties a, ECMaterialProperties b)
{
	return static_cast<ECMaterialProperties>(static_cast<std::underlying_type<ECMaterialProperties>::type>(a) | static_cast<std::underlying_type<ECMaterialProperties>::type>(b));
}

inline ECMaterialProperties operator &(ECMaterialProperties a, ECMaterialProperties b)
{
	return static_cast<ECMaterialProperties>(static_cast<std::underlying_type<ECMaterialProperties>::type>(a) & static_cast<std::underlying_type<ECMaterialProperties>::type>(b));
}

inline ECMaterialProperties operator ^(ECMaterialProperties a, ECMaterialProperties b)
{
	return static_cast<ECMaterialProperties>(static_cast<std::underlying_type<ECMaterialProperties>::type>(a) ^ static_cast<std::underlying_type<ECMaterialProperties>::type>(b));
}

inline ECMaterialProperties operator ~(ECMaterialProperties a)
{
	return static_cast<ECMaterialProperties>(~static_cast<std::underlying_type<ECMaterialProperties>::type>(a));
}

inline ECMaterialProperties &operator |=(ECMaterialProperties &a, ECMaterialProperties b)
{
	return reinterpret_cast<ECMaterialProperties &>(reinterpret_cast<std::underlying_type<ECMaterialProperties>::type &>(a) |= static_cast<std::underlying_type<ECMaterialProperties>::type>(b));
}

inline ECMaterialProperties &operator &=(ECMaterialProperties &a, ECMaterialProperties b)
{
	return reinterpret_cast<ECMaterialProperties &>(reinterpret_cast<std::underlying_type<ECMaterialProperties>::type &>(a) &= static_cast<std::underlying_type<ECMaterialProperties>::type>(b));
}

inline ECMaterialProperties &operator ^=(ECMaterialProperties &a, ECMaterialProperties b)
{
	return reinterpret_cast<ECMaterialProperties &>(reinterpret_cast<std::underlying_type<ECMaterialProperties>::type &>(a) ^= static_cast<std::underlying_type<ECMaterialProperties>::type>(b));
}

#include "CMaterial.h"

#endif