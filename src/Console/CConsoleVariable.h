#ifndef __CCONSOLEVARIABLE_H__
#define __CCONSOLEVARIABLE_H__

#include <functional>
#include <string>

enum class ECConsoleVariableType
{
	Bool,
	Float,
	Int,
	String
};

class CConsoleVariable
{
public:
	CConsoleVariable() {}
	CConsoleVariable(const std::string &name, bool value, const std::function<void (bool &)> &func) : m_type(ECConsoleVariableType::Bool), m_name(name), m_value(value ? "true" : "false"), m_func([func](CConsoleVariable &variable) { bool Value = variable.GetBool(); func(Value); variable.SetBool(Value); }) {}
	CConsoleVariable(const std::string &name, float value, const std::function<void (float &)> &func) : m_type(ECConsoleVariableType::Float), m_name(name), m_value(std::to_string(value)), m_func([this, func](CConsoleVariable &variable) { float Value = variable.GetFloat(); func(Value); variable.SetFloat(Value); }) {}
	CConsoleVariable(const std::string &name, int value, const std::function<void (int &)> &func) : m_type(ECConsoleVariableType::Int), m_name(name), m_value(std::to_string(value)), m_func([this, func](CConsoleVariable &variable) { int Value = variable.GetInt(); func(Value); variable.SetInt(Value); }) {}
	CConsoleVariable(const std::string &name, const std::string &value, const std::function<void (std::string &)> &func) : m_type(ECConsoleVariableType::String), m_name(name), m_value(value), m_func([this, func](CConsoleVariable &variable) { std::string Value = variable.GetString(); func(Value); variable.SetString(Value); }) {}

	ECConsoleVariableType GetType() const { return m_type; }
	void SetType(ECConsoleVariableType type) { m_type = type; ProcessValue(); }
	const std::string &GetName() const { return m_name; }
	void SetName(const std::string &name) { m_name = name; }
	const std::string &GetValue() const { return m_value; }
	void SetValue(const std::string &value) { m_value = value; ProcessValue(); m_func(*this); }
	bool GetBool() const { return (m_value == "true"); }
	void SetBool(bool value) { m_value = (value ? "true" : "false"); }
	float GetFloat() const { return std::stof(m_value); }
	void SetFloat(float value) { m_value = std::to_string(value); }
	int GetInt() const { return std::stoi(m_value); }
	void SetInt(int value) { m_value = std::to_string(value); }
	const std::string &GetString() const { return m_value; }
	void SetString(const std::string &value) { m_value = value; }

private:
	void ProcessValue();

	ECConsoleVariableType m_type;
	std::string m_name;
	std::string m_value;
	std::function<void (CConsoleVariable &)> m_func;
};
#endif