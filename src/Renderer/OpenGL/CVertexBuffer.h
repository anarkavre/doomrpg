#ifndef __CVERTEXBUFFER_H__
#define __CVERTEXBUFFER_H__

#include "Interface/IRenderer.h"

class CIndexBuffer;

class CVertexBuffer
{
public:
	CVertexBuffer(IRenderer *renderer = nullptr, const ECVertexComponent *components = nullptr, unsigned int componentCount = 0, const void *data = nullptr, unsigned int size = 0, bool dynamic = false) { Initialize(renderer, components, componentCount, data, size, dynamic); }
	~CVertexBuffer();

	bool Initialize(IRenderer *renderer, const ECVertexComponent *components, unsigned int componentCount, const void *data, unsigned int size, bool dynamic = false);
	void Bind();
	void Orphan();
	void Update(const void *data, unsigned int offset = 0, unsigned int size = 0);
	void *Map(const char *accessMode);
	void *Map(unsigned int offset, unsigned int length, const char *accessMode);
	void Unmap();
	void Draw(ECDrawMode drawMode, unsigned int start, unsigned int count);
	CIndexBuffer *AddIndexBuffer(const void *data, unsigned int size, bool dynamic = false);
	void RemoveIndexBuffer();

	unsigned int GetSize() const { return m_size; }
	CIndexBuffer *GetIndexBuffer() { return m_indexBuffer; }

	bool IsInitialized() const { return m_initialized; }
	bool IsDynamic() const { return m_dynamic; }

private:
	bool m_initialized{false};
	IRenderer *m_renderer{nullptr};
	unsigned int m_handle{0};
	unsigned int m_size{0};
	bool m_dynamic{false};
	CIndexBuffer *m_indexBuffer{nullptr};
};

#endif