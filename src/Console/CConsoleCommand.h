#ifndef __CCONSOLECOMMAND_H__
#define __CCONSOLECOMMAND_H__

#include <functional>
#include <string>
#include <vector>

class CConsole;

class CConsoleCommand
{
public:
	CConsoleCommand() {}
	CConsoleCommand(const std::string &name, const std::function<void (CConsole &, const std::vector<std::string> &)> &func) : m_name(name), m_func(func) {}

	const std::string &GetName() const { return m_name; }
	void SetName(const std::string &name) { m_name = name; }

	void operator()(CConsole &console, const std::vector<std::string> &arguments) { m_func(console, arguments); }

private:
	std::string m_name;
	std::function<void (CConsole &, const std::vector<std::string> &)> m_func;
};
#endif