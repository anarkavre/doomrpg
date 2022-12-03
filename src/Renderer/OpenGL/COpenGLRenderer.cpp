#include <stdexcept>
#include <vector>

#include "glm/gtc/type_ptr.hpp"

#include "COpenGLRenderer.h"
#include "Console/CConsole.h"
#include "Game/CGame.h"
#include "Window/CWindow.h"

COpenGLRenderer::~COpenGLRenderer()
{
	if (m_game != nullptr)
		m_game->GetGameLoop().UnregisterPresentHandler(*this);

	if (m_GLContext != nullptr)
		SDL_GL_DeleteContext(m_GLContext);
}

bool COpenGLRenderer::Initialize(CGame *game, CWindow *window)
{
	if (!m_initialized && game != nullptr && window != nullptr)
	{
		m_game = game;

		CConsole &console = m_game->GetConsole();

		m_window = window->GetWindow();

		console.Write("Creating GL context...");

		if ((m_GLContext = SDL_GL_CreateContext(m_window)) == nullptr)
			throw std::runtime_error("Could not create GL context");

		console.Write("OK\n");

		glewExperimental = GL_TRUE;

		console.Write("Initializing GLEW...");

		if (glewInit() != GLEW_OK)
			throw std::runtime_error("Could not initialize GLEW");

		console.Write("OK\n");

		SetVsync(true);
		SetDepthTest(false);
		SetCullFace(false);
		SetBlend(false);
		SetBlendMode(ECBlendMode::Interpolative);
		SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		SetClearDepth(1.0f);

		glDepthFunc(GL_LEQUAL);

		m_game->GetGameLoop().RegisterPresentHandler(*this);

		m_initialized = true;
	}

	return m_initialized;
}

bool COpenGLRenderer::Present()
{
	if (!m_initialized)
		return false;

	SDL_GL_SwapWindow(m_window);

	return false;
}

void COpenGLRenderer::Clear(bool colorBuffer, bool depthBuffer)
{
	GLbitfield mask = 0;

	if (colorBuffer)
		mask |= GL_COLOR_BUFFER_BIT;

	if (depthBuffer)
		mask |= GL_DEPTH_BUFFER_BIT;

	glClear(mask);
}

bool COpenGLRenderer::CreateShader(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, unsigned int &handle, unsigned int &errorCode, std::string &errorMessage)
{
	GLuint vertexShader;

	if (CompileShader(GL_VERTEX_SHADER, vertexShaderSource, vertexShader, errorCode, errorMessage) == GL_FALSE)
		return false;

	GLuint fragmentShader;

	if (CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource, fragmentShader, errorCode, errorMessage) == GL_FALSE)
	{
		glDeleteShader(vertexShader);

		return false;
	}

	GLuint program;

	if (LinkProgram(vertexShader, fragmentShader, program, errorCode, errorMessage) == GL_FALSE)
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return false;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	handle = m_shaderPool.Add({ program });

	return true;
}

void COpenGLRenderer::DestroyShader(unsigned int handle)
{
	glDeleteProgram(m_shaderPool.Get(handle).program);
}

void COpenGLRenderer::UseShader(unsigned int handle)
{
	glUseProgram(m_shaderPool.Get(handle).program);
}

unsigned int COpenGLRenderer::GetShaderAttributeLocation(unsigned int handle, const std::string &name)
{
	return glGetAttribLocation(m_shaderPool.Get(handle).program, name.c_str());
}

unsigned int COpenGLRenderer::GetShaderUniformLocation(unsigned int handle, const std::string &name)
{
	return glGetUniformLocation(m_shaderPool.Get(handle).program, name.c_str());
}

void COpenGLRenderer::SetUniform(unsigned int location, bool boolean)
{
	glUniform1i(location, boolean);
}

void COpenGLRenderer::SetUniform(unsigned int location, float decimal)
{
	glUniform1f(location, decimal);
}

void COpenGLRenderer::SetUniform(unsigned int location, int integer)
{
	glUniform1i(location, integer);
}

void COpenGLRenderer::SetUniform(unsigned int location, const glm::mat3 &matrix)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void COpenGLRenderer::SetUniform(unsigned int location, const glm::mat4 &matrix)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void COpenGLRenderer::SetUniform(unsigned int location, const glm::vec3 &vector)
{
	glUniform3fv(location, 1, glm::value_ptr(vector));
}

void COpenGLRenderer::SetUniform(unsigned int location, const glm::vec4 &vector)
{
	glUniform4fv(location, 1, glm::value_ptr(vector));
}

bool COpenGLRenderer::CreateTexture(const void *pixels, unsigned int width, unsigned int height, unsigned int bpp, unsigned int &handle)
{
	GLuint texture;

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	if (bpp == 8)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
	else if (bpp == 24)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	else if (bpp == 32)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	handle = m_texturePool.Add({ GL_TEXTURE_2D, texture });

	return true;
}

bool COpenGLRenderer::CreateTextureArray(const void *pixels, unsigned int width, unsigned int height, unsigned int bpp, unsigned int tileColumnCount, unsigned int tileRowCount, unsigned int &handle)
{
	GLuint texture;

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

	unsigned int tileWidth = width / tileColumnCount;
	unsigned int tileHeight = height / tileRowCount;
	unsigned int tileCount = tileColumnCount * tileRowCount;

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, tileWidth, tileHeight, tileCount);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
	glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, height);

	for (unsigned int y = 0; y < tileRowCount; y++)
	{
		for (unsigned int x = 0; x < tileColumnCount; x++)
		{
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, y * tileColumnCount + x, tileWidth, tileHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, (uint32_t *)pixels + y * tileHeight * width + x * tileWidth);
		}
	}

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);

	handle = m_texturePool.Add({ GL_TEXTURE_2D_ARRAY, texture });

	return true;
}

void COpenGLRenderer::DestroyTexture(unsigned int handle)
{
	glDeleteTextures(1, &m_texturePool.Get(handle).texture);
}

void COpenGLRenderer::BindTexture(unsigned int handle)
{
	glBindTexture(m_texturePool.Get(handle).target, m_texturePool.Get(handle).texture);
}

void COpenGLRenderer::GenerateTextureMipmaps(unsigned int handle)
{
	glGenerateMipmap(m_texturePool.Get(handle).target);
}

void COpenGLRenderer::SetTextureFilterMode(unsigned int handle, ECTextureFilter filter)
{
	if (filter == ECTextureFilter::Point)
	{
		glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else if (filter == ECTextureFilter::Bilinear)
	{
		glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if (filter == ECTextureFilter::Trilinear)
	{
		glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void COpenGLRenderer::SetTextureWrapMode(unsigned int handle, ECTextureCoord coord, ECTextureWrap wrap)
{
	if (coord == ECTextureCoord::U)
	{
		if (wrap == ECTextureWrap::Clamp)
			glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		else if (wrap == ECTextureWrap::Repeat)
			glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	}
	else if (coord == ECTextureCoord::V)
	{
		if (wrap == ECTextureWrap::Clamp)
			glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		else if (wrap == ECTextureWrap::Repeat)
			glTexParameteri(m_texturePool.Get(handle).target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

bool COpenGLRenderer::CreateVertexBuffer(const ECVertexComponent *components, unsigned int componentCount, const void *data, unsigned int size, bool dynamic, unsigned int &handle)
{
	GLuint buffer;

	glGenBuffers(1, &buffer);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, size, data, (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

	GLuint vertexArray;

	glGenVertexArrays(1, &vertexArray);

	glBindVertexArray(vertexArray);

	unsigned int stride = 0;
	unsigned int offset = 0;

	for (unsigned int i = 0; i < componentCount; i++)
	{
		ECVertexComponent component = components[i];

		if (component == ECVertexComponent::Position2D || component == ECVertexComponent::TexCoord2D)
			stride += 2;
		else if (component == ECVertexComponent::Position3D || component == ECVertexComponent::TexCoord3D || component == ECVertexComponent::Normal)
			stride += 3;
		else if (component == ECVertexComponent::Color)
			stride += 4;
	}

	stride *= sizeof(GLfloat);

	for (unsigned int i = 0; i < componentCount; i++)
	{
		ECVertexComponent component = components[i];

		if (component == ECVertexComponent::Position2D || component == ECVertexComponent::TexCoord2D)
		{
			glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
			offset += 2 * sizeof(GLfloat);
		}
		else if (component == ECVertexComponent::Position3D || component == ECVertexComponent::TexCoord3D || component == ECVertexComponent::Normal)
		{
			glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
			offset += 3 * sizeof(GLfloat);
		}
		else if (component == ECVertexComponent::Color)
		{
			glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
			offset += 4 * sizeof(GLfloat);
		}

		glEnableVertexAttribArray(i);
	}

	handle = m_vertexBufferPool.Add({ GL_ARRAY_BUFFER, buffer, vertexArray, static_cast<GLsizeiptr>(size), static_cast<GLenum>(dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) });

	return true;
}

void COpenGLRenderer::DestroyVertexBuffer(unsigned int handle)
{
	glDeleteVertexArrays(1, &m_vertexBufferPool.Get(handle).vertexArray);
	glDeleteBuffers(1, &m_vertexBufferPool.Get(handle).buffer);
}

void COpenGLRenderer::BindVertexBuffer(unsigned int handle)
{
	glBindBuffer(m_vertexBufferPool.Get(handle).target, m_vertexBufferPool.Get(handle).buffer);
}

void COpenGLRenderer::BindVertexArray(unsigned int handle)
{
	glBindVertexArray(m_vertexBufferPool.Get(handle).vertexArray);
}

void COpenGLRenderer::OrphanVertexBuffer(unsigned int handle)
{
	glBufferData(m_vertexBufferPool.Get(handle).target, m_vertexBufferPool.Get(handle).size, nullptr, m_vertexBufferPool.Get(handle).usage);
}

void COpenGLRenderer::UpdateVertexBuffer(unsigned int handle, const void *data, unsigned int offset, unsigned int size)
{
	glBufferSubData(m_vertexBufferPool.Get(handle).target, offset, size, data);
}

void *COpenGLRenderer::MapVertexBuffer(unsigned int handle, const char *accessMode)
{
	GLenum access;

	if (accessMode == "r")
		access = GL_READ_ONLY;
	else if (accessMode == "w")
		access = GL_WRITE_ONLY;
	else
		access = GL_READ_WRITE;

	return glMapBuffer(m_vertexBufferPool.Get(handle).target, access);
}

void *COpenGLRenderer::MapVertexBuffer(unsigned int handle, unsigned int offset, unsigned int length, const char *accessMode)
{
	GLenum access;

	if (accessMode == "r")
		access = GL_READ_ONLY;
	else if (accessMode == "w")
		access = GL_WRITE_ONLY;
	else
		access = GL_READ_WRITE;

	return glMapBufferRange(m_vertexBufferPool.Get(handle).target, offset, length, access);
}

void COpenGLRenderer::UnmapVertexBuffer(unsigned int handle)
{
	glUnmapBuffer(m_vertexBufferPool.Get(handle).target);
}

void COpenGLRenderer::DrawVertexBuffer(unsigned int handle, ECDrawMode drawMode, unsigned int start, unsigned int count)
{
	GLenum mode;

	if (drawMode == ECDrawMode::Triangles)
		mode = GL_TRIANGLES;
	else
		mode = GL_TRIANGLE_STRIP;

	glDrawArrays(mode, start, count);
}

bool COpenGLRenderer::CreateIndexBuffer(const void *data, unsigned int size, bool dynamic, unsigned int &handle)
{
	GLuint buffer;

	glGenBuffers(1, &buffer);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

	handle = m_indexBufferPool.Add({ GL_ELEMENT_ARRAY_BUFFER, buffer, static_cast<GLsizeiptr>(size), static_cast<GLenum>(dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) });

	return true;
}

void COpenGLRenderer::DestroyIndexBuffer(unsigned int handle)
{
	glDeleteBuffers(1, &m_indexBufferPool.Get(handle).buffer);
}

void COpenGLRenderer::BindIndexBuffer(unsigned int handle)
{
	glBindBuffer(m_indexBufferPool.Get(handle).target, m_indexBufferPool.Get(handle).buffer);
}

void COpenGLRenderer::OrphanIndexBuffer(unsigned int handle)
{
	glBufferData(m_indexBufferPool.Get(handle).target, m_indexBufferPool.Get(handle).size, nullptr, m_indexBufferPool.Get(handle).usage);
}

void COpenGLRenderer::UpdateIndexBuffer(unsigned int handle, const void *data, unsigned int offset, unsigned int size)
{
	glBufferSubData(m_indexBufferPool.Get(handle).target, offset, size, data);
}

void *COpenGLRenderer::MapIndexBuffer(unsigned int handle, const char *accessMode)
{
	GLenum access;

	if (accessMode == "r")
		access = GL_READ_ONLY;
	else if (accessMode == "w")
		access = GL_WRITE_ONLY;
	else
		access = GL_READ_WRITE;

	return glMapBuffer(m_indexBufferPool.Get(handle).target, access);
}

void *COpenGLRenderer::MapIndexBuffer(unsigned int handle, unsigned int offset, unsigned int length, const char *accessMode)
{
	GLenum access;

	if (accessMode == "r")
		access = GL_READ_ONLY;
	else if (accessMode == "w")
		access = GL_WRITE_ONLY;
	else
		access = GL_READ_WRITE;

	return glMapBufferRange(m_indexBufferPool.Get(handle).target, offset, length, access);
}

void COpenGLRenderer::UnmapIndexBuffer(unsigned int handle)
{
	glUnmapBuffer(m_indexBufferPool.Get(handle).target);
}

void COpenGLRenderer::DrawIndexBuffer(unsigned int handle, ECDrawMode drawMode, unsigned int start, unsigned int count)
{
	GLenum mode;

	if (drawMode == ECDrawMode::Triangles)
		mode = GL_TRIANGLES;
	else
		mode = GL_TRIANGLE_STRIP;

	glDrawElements(mode, count, GL_UNSIGNED_INT, (GLvoid *)(sizeof(GLuint) * start));
}

void COpenGLRenderer::SetDepthTest(bool depthTest)
{
	if (!m_initialized)
		return;

	if (depthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	m_depthTest = depthTest;
}

void COpenGLRenderer::SetCullFace(bool cullFace)
{
	if (!m_initialized)
		return;

	if (cullFace)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	m_cullFace = cullFace;
}

void COpenGLRenderer::SetBlend(bool blend)
{
	if (!m_initialized)
		return;

	if (blend)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	m_blend = blend;
}

void COpenGLRenderer::SetBlendMode(ECBlendMode blendMode)
{
	if (!m_initialized)
		return;

	if (blendMode == ECBlendMode::None)
		SetBlend(false);
	else
	{
		SetBlend(true);

		if (blendMode == ECBlendMode::Additive)
			glBlendFunc(GL_ONE, GL_ONE);
		else if (blendMode == ECBlendMode::Interpolative)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	m_blendMode = blendMode;
}

void COpenGLRenderer::SetVsync(bool vsync)
{
	if (!m_initialized)
		return;

	SDL_GL_SetSwapInterval(vsync ? 1 : 0);

	m_vsync = vsync;
}

void COpenGLRenderer::SetViewport(int x, int y, int width, int height)
{
	if (!m_initialized)
		return;

	glViewport(x, y, width, height);

	m_viewport[0] = x;
	m_viewport[1] = y;
	m_viewport[2] = width;
	m_viewport[3] = height;
}

void COpenGLRenderer::SetClearColor(const glm::vec4 &clearColor)
{
	if (!m_initialized)
		return;

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

	m_clearColor = clearColor;
}

void COpenGLRenderer::SetClearDepth(float clearDepth)
{
	if (!m_initialized)
		return;

	glClearDepth(clearDepth);

	m_clearDepth = clearDepth;
}

GLint COpenGLRenderer::CompileShader(GLenum type, const std::string &source, GLuint &shader, unsigned int &errorCode, std::string &errorMessage)
{
	CConsole &console = m_game->GetConsole();

	if (type == GL_VERTEX_SHADER)
		console.Write("Compiling vertex shader...");
	else if (type == GL_FRAGMENT_SHADER)
		console.Write("Compiling fragment shader...");

	shader = glCreateShader(type);
	const GLchar *str = source.c_str();
	glShaderSource(shader, 1, &str, nullptr);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		console.Write("Failed\n");

		GLint infoLogLength;
		GLchar *infoLog = nullptr;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			infoLog = new GLchar[infoLogLength];
			glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog);
		}
		
		if (type == GL_VERTEX_SHADER)
			errorCode = 1;
		else if (type == GL_FRAGMENT_SHADER)
			errorCode = 2;

		errorMessage = (infoLog != nullptr ? std::string(infoLog) : "");

		if (infoLog != nullptr)
			delete[] infoLog;

		glDeleteShader(shader);

		return GL_FALSE;
	}

	console.Write("OK\n");

	return GL_TRUE;
}

GLint COpenGLRenderer::LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint &program, unsigned int &errorCode, std::string &errorMessage)
{
	CConsole &console = m_game->GetConsole();

	console.Write("Linking program...");

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		console.Write("Failed\n");

		GLint infoLogLength;
		GLchar *infoLog = nullptr;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			infoLog = new GLchar[infoLogLength];
			glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog);
		}

		errorCode = 3;
		errorMessage = (infoLog != nullptr ? std::string(infoLog) : "");

		if (infoLog != nullptr)
			delete[] infoLog;

		glDeleteProgram(program);

		return GL_FALSE;
	}

	console.Write("OK\n");

	return GL_TRUE;
}