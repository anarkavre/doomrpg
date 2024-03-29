#ifndef __CRESOURCE_H__
#define __CRESOURCE_H__

#include <fstream>
#include <string>

class CGame;

enum class ECResourceType
{
	Undefined,
	Animation,
	Font,
	Map,
	Music,
	Shader,
	Sound,
	Texture
};

class CResource
{
public:
	CResource(CGame *game) : m_game(game) {}
	CResource(CGame *game, unsigned int handle, const std::string &name, const std::string &path, ECResourceType type, bool openFile = true);
	~CResource();

	const std::string &GetName() const { return m_name; }
	const std::string &GetPath() const { return m_path; }
	ECResourceType GetType() const { return m_type; }

private:
	std::ifstream m_ifstream;
	unsigned long m_size{0};

protected:
	void CloseFile();

	unsigned char *GetData(unsigned char *data, unsigned long size, unsigned long *count = nullptr);
	unsigned long GetSize() const { return m_size; }

	CGame *m_game{nullptr};
	unsigned int m_handle{UINT_MAX};
	std::string m_name;
	std::string m_path;
	ECResourceType m_type{ECResourceType::Undefined};
};

#endif