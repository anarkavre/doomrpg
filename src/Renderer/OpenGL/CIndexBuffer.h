#ifndef __CINDEXBUFFER_H__
#define __CINDEXBUFFER_H__

#include "Interface/IRenderer.h"

class CIndexBuffer
{
public:
	CIndexBuffer(IRenderer *renderer = nullptr, const void *data = nullptr, unsigned int size = 0, bool dynamic = false) { Initialize(renderer, data, size, dynamic); }
	~CIndexBuffer();

	bool Initialize(IRenderer *renderer, const void *data, unsigned int size, bool dynamic = false);
	void Bind();
	void Orphan();
	void Update(const void *data, unsigned int offset = 0, unsigned int size = 0);
	void *Map(const char *accessMode);
	void *Map(unsigned int offset, unsigned int length, const char *accessMode);
	void Unmap();

	unsigned int GetSize() const { return m_size; }

	bool IsInitialized() const { return m_initialized; }
	bool IsDynamic() const { return m_dynamic; }

private:
	bool m_initialized{false};
	IRenderer *m_renderer{nullptr};
	unsigned int m_handle{0};
	unsigned int m_size{0};
	bool m_dynamic{false};
};

#endif