#include "CIndexBuffer.h"

CIndexBuffer::~CIndexBuffer()
{
	if (m_initialized)
		m_renderer->DestroyIndexBuffer(m_handle);
}

bool CIndexBuffer::Initialize(IRenderer *renderer, const void *data, unsigned int size, bool dynamic)
{
	if (!m_initialized && renderer != nullptr)
	{
		m_renderer = renderer;
		m_size = size;
		m_dynamic = dynamic;

		m_renderer->CreateIndexBuffer(data, size, dynamic, m_handle);

		m_initialized = true;
	}

	return m_initialized;
}

void CIndexBuffer::Bind()
{
	if (!m_initialized)
		return;

	m_renderer->BindIndexBuffer(m_handle);
}

void CIndexBuffer::Orphan()
{
	if (!m_initialized)
		return;

	m_renderer->OrphanIndexBuffer(m_handle);
}

void CIndexBuffer::Update(const void *data, unsigned int offset, unsigned int size)
{
	if (!m_initialized)
		return;

	m_renderer->BindIndexBuffer(m_handle);
	m_renderer->UpdateIndexBuffer(m_handle, data, offset, (size > 0 ? size : m_size));
}

void *CIndexBuffer::Map(const char *accessMode)
{
	if (!m_initialized)
		return nullptr;

	m_renderer->BindIndexBuffer(m_handle);

	return m_renderer->MapIndexBuffer(m_handle, accessMode);
}

void *CIndexBuffer::Map(unsigned int offset, unsigned int length, const char *accessMode)
{
	if (!m_initialized)
		return nullptr;

	m_renderer->BindIndexBuffer(m_handle);

	return m_renderer->MapIndexBuffer(m_handle, offset, length, accessMode);
}

void CIndexBuffer::Unmap()
{
	if (!m_initialized)
		return;

	m_renderer->UnmapIndexBuffer(m_handle);
}