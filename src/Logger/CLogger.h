#ifndef __CLOGGER_H__
#define __CLOGGER_H__

#include <fstream>
#include <string>

class CGame;

class CLogger
{
public:
	CLogger() {}
	CLogger(CGame *game, const std::string &name) { Initialize(game, name); }
	~CLogger();

	bool Initialize(CGame *game, const std::string &name);
	void Write(const std::string &text);

	const std::string &GetName() const { return m_name; }

	bool IsInitialized() const { return m_initialized; }

private:
	bool m_initialized{false};
	std::string m_name;
	std::ofstream m_ofstream;
};

#endif