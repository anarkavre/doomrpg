#include "CIndexBuffer.h"
#include "CVertexBuffer.h"

CVertexBuffer::~CVertexBuffer()
{
	if (m_initialized)
	{
		if (m_indexBuffer != nullptr)
			delete m_indexBuffer;

		m_renderer->DestroyVertexBuffer(m_handle);
	}
}

bool CVertexBuffer::Initialize(IRenderer *renderer, const ECVertexComponent *components, unsigned int componentCount, const void *data, unsigned int size, bool dynamic)
{
	if (!m_initialized && renderer != nullptr)
	{
		m_renderer = renderer;

		m_renderer->CreateVertexBuffer(components, componentCount, data, size, dynamic, m_handle);

		m_initialized = true;
	}

	return m_initialized;
}

void CVertexBuffer::Bind()
{
	if (!m_initialized)
		return;

	m_renderer->BindVertexArray(m_handle);
}

void CVertexBuffer::Orphan()
{
	if (!m_initialized)
		return;

	m_renderer->OrphanVertexBuffer(m_handle);
}

void CVertexBuffer::Update(const void *data, unsigned int offset, unsigned int size)
{
	if (!m_initialized)
		return;

	m_renderer->BindVertexBuffer(m_handle);
	m_renderer->UpdateVertexBuffer(m_handle, data, offset, (size > 0 ? size : m_size));
}

void *CVertexBuffer::Map(const char *accessMode)
{
	if (!m_initialized)
		return nullptr;

	m_renderer->BindVertexBuffer(m_handle);

	return m_renderer->MapVertexBuffer(m_handle, accessMode);
}

void *CVertexBuffer::Map(unsigned int offset, unsigned int length, const char *accessMode)
{
	if (!m_initialized)
		return nullptr;

	m_renderer->BindVertexBuffer(m_handle);

	return m_renderer->MapVertexBuffer(m_handle, offset, length, accessMode);
}

void CVertexBuffer::Unmap()
{
	if (!m_initialized)
		return;

	m_renderer->UnmapVertexBuffer(m_handle);
}

void CVertexBuffer::Draw(ECDrawMode drawMode, unsigned int start, unsigned int count)
{
	if (!m_initialized)
		return;

	if (m_indexBuffer != nullptr)
		m_renderer->DrawIndexBuffer(m_handle, drawMode, start, count);
	else
		m_renderer->DrawVertexBuffer(m_handle, drawMode, start, count);
}

CIndexBuffer *CVertexBuffer::AddIndexBuffer(const void *data, unsigned int size, bool dynamic)
{
	if (!m_initialized)
		return nullptr;

	if (m_indexBuffer != nullptr)
		delete m_indexBuffer;

	m_indexBuffer = new CIndexBuffer(m_renderer, data, size, dynamic);
	
	return m_indexBuffer;
}

void CVertexBuffer::RemoveIndexBuffer()
{
	if (!m_initialized)
		return;

	if (m_indexBuffer != nullptr)
	{
		delete m_indexBuffer;
		m_indexBuffer = nullptr;
	}
}