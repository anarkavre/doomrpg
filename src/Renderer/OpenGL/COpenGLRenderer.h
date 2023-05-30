#ifndef __COPENGLRENDERER_H__
#define __COPENGLRENDERER_H__

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "SDL.h"

#include "Interface/IRenderer.h"
#include "Pool/CPool.h"

class CGame;
class CWindow;

struct OpenGLShaderData
{
	GLuint program;
};

struct OpenGLTextureData
{
	GLenum target;
	GLuint texture;
};

struct OpenGLVertexBufferData
{
	GLenum target;
	GLuint buffer;
	GLuint vertexArray;
	GLsizeiptr size;
	GLenum usage;
};

struct OpenGLIndexBufferData
{
	GLenum target;
	GLuint buffer;
	GLsizeiptr size;
	GLenum usage;
};

class COpenGLRenderer : public IRenderer
{
public:
	COpenGLRenderer() : IRenderer(ECRendererType::OpenGL, "OpenGL") {}
	COpenGLRenderer(CGame *game, CWindow *window) : IRenderer(ECRendererType::OpenGL, "OpenGL") { Initialize(game, window); }
	~COpenGLRenderer();

	bool Initialize(CGame *game, CWindow *window);
	bool Present();

	void Clear(bool colorBuffer = true, bool depthBuffer = true);

	bool CreateShader(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, unsigned int &handle, unsigned int &errorCode, std::string &errorMessage);
	void DestroyShader(unsigned int handle);
	void UseShader(unsigned int handle);
	unsigned int GetShaderAttributeLocation(unsigned int handle, const std::string &name);
	unsigned int GetShaderUniformLocation(unsigned int handle, const std::string &name);
	void SetUniform(unsigned int location, bool boolean);
	void SetUniform(unsigned int location, float decimal);
	void SetUniform(unsigned int location, int integer);
	void SetUniform(unsigned int location, const glm::mat3 &matrix);
	void SetUniform(unsigned int location, const glm::mat4 &matrix);
	void SetUniform(unsigned int location, const glm::vec3 &vector);
	void SetUniform(unsigned int location, const glm::vec4 &vector);

	bool CreateTexture(const void *pixels, unsigned int width, unsigned int height, unsigned int bpp, unsigned int &handle);
	bool CreateTextureArray(const void *pixels, unsigned int width, unsigned int height, unsigned int bpp, unsigned int tileColumnCount, unsigned int tileRowCount, unsigned int &handle);
	void DestroyTexture(unsigned int handle);
	void BindTexture(unsigned int handle);
	void GenerateTextureMipmaps(unsigned int handle);
	void SetTextureFilterMode(unsigned int handle, ECTextureFilter filter);
	void SetTextureWrapMode(unsigned int handle, ECTextureCoord coord, ECTextureWrap wrap);

	bool CreateVertexBuffer(const ECVertexComponent *components, unsigned int componentCount, const void *data, unsigned int size, bool dynamic, unsigned int &handle);
	void DestroyVertexBuffer(unsigned int handle);
	void BindVertexBuffer(unsigned int handle);
	void BindVertexArray(unsigned int handle);
	void OrphanVertexBuffer(unsigned int handle);
	void UpdateVertexBuffer(unsigned int handle, const void *data, unsigned int offset, unsigned int size);
	void *MapVertexBuffer(unsigned int handle, const char *accessMode);
	void *MapVertexBuffer(unsigned int handle, unsigned int offset, unsigned int length, const char *accessMode);
	void UnmapVertexBuffer(unsigned int handle);
	void DrawVertexBuffer(unsigned int handle, ECDrawMode drawMode, unsigned int start, unsigned int count);

	bool CreateIndexBuffer(const void *data, unsigned int size, bool dynamic, unsigned int &handle);
	void DestroyIndexBuffer(unsigned int handle);
	void BindIndexBuffer(unsigned int handle);
	void OrphanIndexBuffer(unsigned int handle);
	void UpdateIndexBuffer(unsigned int handle, const void *data, unsigned int offset, unsigned int size);
	void *MapIndexBuffer(unsigned int handle, const char *accessMode);
	void *MapIndexBuffer(unsigned int handle, unsigned int offset, unsigned int length, const char *accessMode);
	void UnmapIndexBuffer(unsigned int handle);
	void DrawIndexBuffer(unsigned int handle, ECDrawMode drawMode, unsigned int start, unsigned int count);

	SDL_GLContext GetGLContext() const { return m_GLContext; }
	void SetDepthTest(bool depthTest);
	void SetCullFace(bool cullFace);
	void SetBlend(bool blend);
	void SetBlendMode(ECBlendMode blendMode);
	void SetVsync(bool vsync);
	void SetViewport(int x, int y, int width, int height);
	void SetClearColor(const glm::vec4 &clearColor);
	void SetClearDepth(float clearDepth);

	bool IsInitialized() const { return m_initialized; }

private:
	GLint CompileShader(GLenum type, const std::string &source, GLuint &shader, unsigned int &errorCode, std::string &errorMessage);
	GLint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint &program, unsigned int &errorCode, std::string &errorMessage);

	bool m_initialized{false};
	CGame *m_game{nullptr};
	SDL_Window *m_window{nullptr};
	SDL_GLContext m_GLContext{nullptr};
	CPool<OpenGLShaderData> m_shaderPool;
	CPool<OpenGLTextureData> m_texturePool;
	CPool<OpenGLVertexBufferData> m_vertexBufferPool;
	CPool<OpenGLIndexBufferData> m_indexBufferPool;
};

#endif
