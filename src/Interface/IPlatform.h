#ifndef __IPLATFORM_H__
#define __IPLATFORM_H__

#include <string>

enum class ECPlatformType
{
	SDL
};

class IPlatform
{
public:
	IPlatform(ECPlatformType type, const std::string name) : m_type(type), m_name(name) {}
	virtual ~IPlatform() {}

	virtual unsigned int GetTicks() { return 0; }

protected:
	ECPlatformType m_type;
	std::string m_name;
};
#endif