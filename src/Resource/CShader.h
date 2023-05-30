#ifndef __CSHADER_H__
#define __CSHADER_H__

#include <unordered_map>

#include "glm/glm.hpp"

#include "CResource.h"

class IRenderer;

class CShader : public CResource
{
public:
	CShader(CGame *game, unsigned int handle, const std::string &name);
	~CShader();

	void Use();

	unsigned int GetAttributeLocation(const std::string &name);
	unsigned int GetUniformLocation(const std::string &name);
	void SetUniform(const std::string &name, bool boolean);
	void SetUniform(const std::string &name, float decimal);
	void SetUniform(const std::string &name, int integer);
	void SetUniform(const std::string &name, const glm::mat3 &matrix);
	void SetUniform(const std::string &name, const glm::mat4 &matrix);
	void SetUniform(const std::string &name, const glm::vec3 &vector);
	void SetUniform(const std::string &name, const glm::vec4 &vector);

	static const std::string &GetResourcePath() { return m_resourcePath; }

private:
	IRenderer *m_renderer{nullptr};
	unsigned int m_handle{0};
	std::unordered_map<std::string, unsigned int> m_attributeLocations, m_uniformLocations;

	static const std::string m_resourcePath;
};

#endif