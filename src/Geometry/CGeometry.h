#ifndef __CGEOMETRY_H__
#define __CGEOMETRY_H__

#include <string>
#include <vector>

#include "Renderer/VertexComponents.h"

class CGame;

class CGeometry
{
public:
	CGeometry(CGame *game, unsigned int handle, unsigned int id, const std::string &name) : m_game(game), m_handle(handle), m_id(id), m_name(name) {}
	~CGeometry();

	unsigned int GetID() const { return m_id; }
	const std::string &GetName() const { return m_name; }
	const std::vector<ECVertexComponent> &GetVertexComponents() const { return m_vertexComponents; }
	const std::vector<float> &GetVertexData() const { return m_vertexData; }
	const std::vector<unsigned int> &GetIndexData() const { return m_indexData; }
	unsigned int GetVertexCount() const { return m_vertexCount; }

protected:
	std::vector<ECVertexComponent> m_vertexComponents;
	std::vector<float> m_vertexData;
	std::vector<unsigned int> m_indexData;
	unsigned int m_vertexCount{0};

private:
	CGame *m_game{nullptr};
	unsigned int m_handle{0};
	unsigned int m_id{0};
	std::string m_name;
};

#endif