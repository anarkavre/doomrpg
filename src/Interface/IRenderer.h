#ifndef __IRENDERER_H__
#define __IRENDERER_H__

#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "IPresentHandler.h"
#include "Renderer/VertexComponents.h"

enum class ECRendererType
{
	OpenGL
};

enum class ECTextureFilter
{
	Point,
	Bilinear,
	Trilinear
};

enum class ECTextureCoord
{
	U,
	V
};

enum class ECTextureWrap
{
	Clamp,
	Repeat
};

enum class ECDrawMode
{
	Triangles,
	TriangleStrip
};

enum class ECBlendMode
{
	None,
	Additive,
	Interpolative
};

class IRenderer : public IPresentHandler
{
public:
	IRenderer(ECRendererType type, const std::string &name) : m_type(type), m_name(name) {}
	virtual ~IRenderer() {}

	bool Present() { return false; }

	virtual void Clear(bool colorBuffer = true, bool depthBuffer = true) {}

	virtual bool CreateShader(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, unsigned int &handle, unsigned int &errorCode, std::string &errorMessage) { return false; }
	virtual void DestroyShader(unsigned int handle) {}
	virtual void UseShader(unsigned int handle) {}
	virtual unsigned int GetShaderAttributeLocation(unsigned int handle, const std::string &name) { return 0; }
	virtual unsigned int GetShaderUniformLocation(unsigned int handle, const std::string &name) { return 0; }
	virtual void SetUniform(unsigned int location, bool boolean) {}
	virtual void SetUniform(unsigned int location, float decimal) {}
	virtual void SetUniform(unsigned int location, int integer) {}
	virtual void SetUniform(unsigned int location, const glm::mat3 &matrix) {}
	virtual void SetUniform(unsigned int location, const glm::mat4 &matrix) {}
	virtual void SetUniform(unsigned int location, const glm::vec3 &vector) {}
	virtual void SetUniform(unsigned int location, const glm::vec4 &vector) {}

	virtual bool CreateTexture(const void *pixels, unsigned int width, unsigned int height, unsigned int bpp, unsigned int &handle) { return false; }
	virtual bool CreateTextureArray(const void *pixels, unsigned int width, unsigned int height, unsigned int bpp, unsigned int tileColumnCount, unsigned int tileRowCount, unsigned int &handle) { return false; }
	virtual void DestroyTexture(unsigned int handle) {}
	virtual void BindTexture(unsigned int handle) {}
	virtual void GenerateTextureMipmaps(unsigned int handle) {}
	virtual void SetTextureFilterMode(unsigned int handle, ECTextureFilter filter) {}
	virtual void SetTextureWrapMode(unsigned int handle, ECTextureCoord coord, ECTextureWrap wrap) {}

	virtual bool CreateVertexBuffer(const ECVertexComponent *components, unsigned int componentCount, const void *data, unsigned int size, bool dynamic, unsigned int &handle) { return false; }
	virtual void DestroyVertexBuffer(unsigned int handle) {}
	virtual void BindVertexBuffer(unsigned int handle) {}
	virtual void BindVertexArray(unsigned int handle) {}
	virtual void OrphanVertexBuffer(unsigned int handle) {}
	virtual void UpdateVertexBuffer(unsigned int handle, const void *data, unsigned int offset, unsigned int size) {}
	virtual void *MapVertexBuffer(unsigned int handle, const char *accessMode) { return nullptr; }
	virtual void *MapVertexBuffer(unsigned int handle, unsigned int offset, unsigned int length, const char *accessMode) { return nullptr; }
	virtual void UnmapVertexBuffer(unsigned int handle) {}
	virtual void DrawVertexBuffer(unsigned int handle, ECDrawMode drawMode, unsigned int start, unsigned int count) {}

	virtual bool CreateIndexBuffer(const void *data, unsigned int size, bool dynamic, unsigned int &handle) { return false; }
	virtual void DestroyIndexBuffer(unsigned int handle) {}
	virtual void BindIndexBuffer(unsigned int handle) {}
	virtual void OrphanIndexBuffer(unsigned int handle) {}
	virtual void UpdateIndexBuffer(unsigned int handle, const void *data, unsigned int offset, unsigned int size) {}
	virtual void *MapIndexBuffer(unsigned int handle, const char *accessMode) { return nullptr; }
	virtual void *MapIndexBuffer(unsigned int handle, unsigned int offset, unsigned int length, const char *accessMode) { return nullptr; }
	virtual void UnmapIndexBuffer(unsigned int handle) {}
	virtual void DrawIndexBuffer(unsigned int handle, ECDrawMode drawMode, unsigned int start, unsigned int count) {}

	ECRendererType GetType() const { return m_type; }
	const std::string &GetName() const { return m_name; }

	const glm::mat4 &GetProjectionMatrix() const { return m_projectionMatrix; }
	void SetProjectionMatrix(const glm::mat4 &projectionMatrix) { m_projectionMatrix = projectionMatrix; }
	const glm::mat4 &GetOrthoMatrix() const { return m_orthoMatrix; }
	void SetOrthoMatrix(float left, float right, float bottom, float top) { m_orthoMatrix = glm::ortho(left, right, bottom, top); }
	const glm::mat4 &GetViewMatrix() const { return m_viewMatrix; }
	void SetViewMatrix(const glm::mat4 &viewMatrix) { m_viewMatrix = viewMatrix; }
	bool GetDepthTest() const { return m_depthTest; }
	virtual void SetDepthTest(bool depthTest) { m_depthTest = depthTest; }
	bool GetCullFace() const { return m_cullFace; }
	virtual void SetCullFace(bool cullFace) { m_cullFace = cullFace; }
	bool GetBlend() const { return m_blend; }
	virtual void SetBlend(bool blend) { m_blend = blend; }
	ECBlendMode GetBlendMode() const { return m_blendMode; }
	virtual void SetBlendMode(ECBlendMode blendMode) { m_blendMode = blendMode; }
	bool GetVsync() const { return m_vsync; }
	virtual void SetVsync(bool vsync) { m_vsync = vsync; }
	const int *GetViewport() const { return m_viewport; }
	int GetViewportX() const { return m_viewport[0]; }
	int GetViewportY() const { return m_viewport[1]; }
	int GetViewportWidth() const { return m_viewport[2]; }
	int GetViewportHeight() const { return m_viewport[3]; }
	void SetViewport(const int *viewport) { if (viewport != nullptr) SetViewport(viewport[0], viewport[1], viewport[2], viewport[3]); }
	virtual void SetViewport(int x, int y, int width, int height) { m_viewport[0] = x; m_viewport[1] = y; m_viewport[2] = width; m_viewport[3] = height; }
	const glm::vec4 &GetClearColor() const { return m_clearColor; }
	virtual void SetClearColor(const glm::vec4 &clearColor) { m_clearColor = clearColor; }
	float GetClearDepth() const { return m_clearDepth; }
	virtual void SetClearDepth(float clearDepth) { m_clearDepth = clearDepth; }

protected:
	ECRendererType m_type;
	std::string m_name;
	glm::mat4 m_projectionMatrix{0.0f}, m_orthoMatrix{0.0f}, m_viewMatrix{0.0f};
	bool m_depthTest{false};
	bool m_cullFace{false};
	bool m_blend{false};
	ECBlendMode m_blendMode{ECBlendMode::None};
	bool m_vsync{false};
	int m_viewport[4]{0};
	glm::vec4 m_clearColor{0.0f, 0.0f, 0.0f, 1.0f};
	float m_clearDepth{1.0f};
};

#endif