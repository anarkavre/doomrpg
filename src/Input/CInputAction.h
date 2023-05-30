#ifndef __CINPUTACTION_H__
#define __CINPUTACTION_H__

#include <string>

class CInputAction
{
public:
	CInputAction() {}
	CInputAction(const std::string &name) : m_name(name) {}

	const std::string &GetName() const { return m_name; }
	void SetName(const std::string &name) { m_name = name; }
	bool GetPressed() const { m_pressed; }
	void SetPressed(bool pressed) { m_pressed = pressed; }

	bool IsPressed() const { return m_pressed; }

private:
	std::string m_name;
	bool m_pressed{false};
};

#endif