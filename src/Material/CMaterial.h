#ifndef __CMATERIAL_H__
#define __CMATERIAL_H__

#include <string>

#include "Material.h"
#include "Interface/IRenderer.h"

class CGame;
class CShader;
class CTexture;

class CMaterial
{
public:
	CMaterial(CGame *game, unsigned int handle, unsigned int id, const std::string &name) : m_game(game), m_handle(handle), m_id(id), m_name(name) {}
	~CMaterial();

	unsigned int GetID() const { return m_id; }
	const std::string &GetName() const { return m_name; }
	CShader *GetShader() const { return m_shader.get(); }
	void SetShader(std::shared_ptr<CShader> shader) { m_shader = std::move(shader); }
	CTexture *GetTexture() const { return m_texture.get(); }
	void SetTexture(std::shared_ptr<CTexture> texture) { m_texture = std::move(texture); }
	ECMaterialProperties GetProperties() const { return m_properties; }
	void SetProperties(ECMaterialProperties properties) { m_properties = properties; }
	ECBlendMode GetBlendMode() const { return m_blendMode; }
	void SetBlendMode(ECBlendMode blendMode) { m_blendMode = blendMode; }

private:
	CGame *m_game{nullptr};
	unsigned int m_handle{0};
	unsigned int m_id{0};
	std::string m_name;
	std::shared_ptr<CShader> m_shader;
	std::shared_ptr<CTexture> m_texture;
	ECMaterialProperties m_properties{ECMaterialProperties::None};
	ECBlendMode m_blendMode{ECBlendMode::None};
};

#endif
