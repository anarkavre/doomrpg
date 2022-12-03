#ifndef __CPOOL_H__
#define __CPOOL_H__

#include <stack>
#include <vector>

template <class T>
class CPool
{
public:
	CPool(unsigned int capacity = 8) { m_pool.reserve(capacity); }

	unsigned int Add(const T &object);
	void Remove(unsigned int handle);

	const std::vector<T> &GetPool() const { return m_pool; }

	unsigned int GetSize() const { return m_pool.size(); }
	unsigned int GetCapactiy() const { return m_pool.capacity(); }
	void SetCapacity(unsigned int capacity) { m_pool.reserve(capacity); }

	T &Get(unsigned int handle) { return m_pool[handle]; }
	void Set(unsigned int handle, const T &object) { m_pool[handle] = object; }

	bool IsEmpty() const { return m_pool.empty(); }

private:
	std::stack<unsigned int> m_handles;
	std::vector<T> m_pool;
};

template <class T>
unsigned int CPool<T>::Add(const T &object)
{
	bool handleAvailable = !m_handles.empty();
	unsigned int handle;

	if (handleAvailable)
	{
		handle = m_handles.top();
		m_handles.pop();
		m_pool[handle] = object;
	}
	else
	{
		handle = m_pool.size();
		m_pool.push_back(object);
	}

	return handle;
}

template <class T>
void CPool<T>::Remove(unsigned int handle)
{
	if (m_pool.empty())
		return;

	m_pool[handle] = T();
	m_handles.push(handle);
}

#endif