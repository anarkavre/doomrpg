#include <algorithm>
#include <stdexcept>

#include "CShader.h"
#include "Game/CGame.h"
#include "Interface/IRenderer.h"

CShader::CShader(CGame *game, unsigned int handle, const std::string &name) : CResource(game, handle, name, GetResourcePath(), ECResourceType::Shader), m_renderer(game->GetRenderer())
{
	unsigned char *data = new unsigned char[GetSize()];
	std::string source(reinterpret_cast<char *>(GetData(data, GetSize())), GetSize());
	delete[] data;
	size_t vertTagPosition = source.find(".vert"), fragTagPosition = source.find(".frag");
	std::string vertexShaderSource = source.substr(vertTagPosition + 5, fragTagPosition - 5), fragmentShaderSource = source.substr(fragTagPosition + 5);
	size_t versionPosition = fragmentShaderSource.find("#version");
	size_t newlinePosition = fragmentShaderSource.find('\n', versionPosition + 8);
	size_t noneNewlinePosition = fragmentShaderSource.find_first_not_of("\r\n", newlinePosition + 1);
	std::string version = fragmentShaderSource.substr(0, noneNewlinePosition);
	fragmentShaderSource = version + "#line " + std::to_string(count(vertexShaderSource.begin(), vertexShaderSource.end(), '\n') + count(version.begin(), version.end(), '\n')) + (version[version.length() - 2] == '\r' && version[version.length() - 1] == '\n' ? "\r\n" : "\n") + fragmentShaderSource.substr(noneNewlinePosition);
	unsigned int errorCode;
	std::string errorMessage;

	if (!m_renderer->CreateShader(vertexShaderSource, fragmentShaderSource, m_handle, errorCode, errorMessage))
	{
		if (errorCode == 1)
			throw std::runtime_error("Could not compile vertex shader in \"" + m_path + m_name + "\"\n" + errorMessage);
		else if (errorCode == 2)
			throw std::runtime_error("Could not compile fragment shader in \"" + m_path + m_name + "\"\n" + errorMessage);
		else if (errorCode == 3)
			throw std::runtime_error("Could not link program in \"" + m_path + m_name + "\"\n" + errorMessage);
	}
}

CShader::~CShader()
{
	m_renderer->DestroyShader(m_handle);
}

void CShader::Use()
{
	m_renderer->UseShader(m_handle);
}

unsigned int CShader::GetAttributeLocation(const std::string &name)
{
	if (m_attributeLocations.count(name) == 0)
		m_attributeLocations[name] = m_renderer->GetShaderAttributeLocation(m_handle, name);

	return m_attributeLocations[name];
}

unsigned int CShader::GetUniformLocation(const std::string &name)
{
	if (m_uniformLocations.count(name) == 0)
		m_uniformLocations[name] = m_renderer->GetShaderUniformLocation(m_handle, name);

	return m_uniformLocations[name];
}

void CShader::SetUniform(const std::string &name, bool boolean)
{
	m_renderer->SetUniform(GetUniformLocation(name), boolean);
}

void CShader::SetUniform(const std::string &name, float decimal)
{
	m_renderer->SetUniform(GetUniformLocation(name), decimal);
}

void CShader::SetUniform(const std::string &name, int integer)
{
	m_renderer->SetUniform(GetUniformLocation(name), integer);
}

void CShader::SetUniform(const std::string &name, const glm::mat3 &matrix)
{
	m_renderer->SetUniform(GetUniformLocation(name), matrix);
}

void CShader::SetUniform(const std::string &name, const glm::mat4 &matrix)
{
	m_renderer->SetUniform(GetUniformLocation(name), matrix);
}

void CShader::SetUniform(const std::string &name, const glm::vec3 &vector)
{
	m_renderer->SetUniform(GetUniformLocation(name), vector);
}

void CShader::SetUniform(const std::string &name, const glm::vec4 &vector)
{
	m_renderer->SetUniform(GetUniformLocation(name), vector);
}

const std::string CShader::m_resourcePath = "shaders/";