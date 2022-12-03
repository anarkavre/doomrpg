#ifndef __CCONFIG_H__
#define __CCONFIG_H__

#include <map>
#include <string>

class CGame;

class CConfig
{
public:
	CConfig() : m_initialized(false) {}
	CConfig(CGame *game, const std::string &name) : m_initialized(false) { Initialize(game, name); }
	~CConfig();

	bool Initialize(CGame *game, const std::string &name);
	void Save();

	const std::string &GetName() const { return m_name; }
	const std::string &Get(const std::string &name) { return m_variables[name]; }
	void Set(const std::string &name, const std::string &value) { m_variables[name] = value; }
	unsigned int GetCount() const { return m_variables.size(); }

	bool IsInitialized() const { return m_initialized; }

private:
	bool m_initialized;
	std::string m_name;
	std::map<std::string, std::string> m_variables;
};

#endif