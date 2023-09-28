#include "glm/gtx/transform.hpp"

#include "CRenderSystem.h"
#include "Game/CGame.h"
#include "Object/CObject.h"
#include "Renderer/OpenGL/CIndexBuffer.h"
#include "Renderer/OpenGL/CVertexBuffer.h"
#include "Resource/CFont.h"
#include "Resource/CShader.h"
#include "Resource/CTexture.h"

struct VertexDataBucket
{
	std::vector<float> vertexData;
	unsigned int vertexCount;
};

struct UIElementVertexData
{
	VertexComponent::Position2D position;
	VertexComponent::TexCoord2D texCoord;
};

struct TextVertexData
{
	VertexComponent::Position2D position;
	VertexComponent::TexCoord2D texCoord;
};

struct ConsoleVertexData
{
	VertexComponent::Position2D position;
	VertexComponent::TexCoord2D texCoord;
};

CRenderSystem::~CRenderSystem()
{
	if (m_initialized)
	{
		m_game->GetGameLoop().UnregisterSystem(*this);
		m_game->GetGameLoop().UnregisterRenderHandler(*this);

		for (auto it = m_sceneEntityBatches.begin(); it != m_sceneEntityBatches.end(); ++it)
			delete (*it).first;

		if (m_uiElementVertexBuffer != nullptr)
			delete m_uiElementVertexBuffer;

		if (m_textVertexBuffer != nullptr)
			delete m_textVertexBuffer;

		if (m_consoleVertexBuffer != nullptr)
			delete m_consoleVertexBuffer;
	}
}

bool CRenderSystem::Initialize(CGame *game)
{
	if (ISystem::Initialize(game))
	{
		m_renderer = m_game->GetRenderer();

		InitScene();
		InitUI();
		InitText();
		InitConsole();

		m_game->GetGameLoop().RegisterSystem(*this);
		m_game->GetGameLoop().RegisterRenderHandler(*this);

		m_game->GetConsole().RegisterVariable("r_texture_filter", 1, [this](int &value) { int textureFilter = glm::clamp(value, 0, 2); value = textureFilter; SetTextureFilter(static_cast<ECTextureFilter>(textureFilter)); });

		m_initialized = true;
	}

	return m_initialized;
}

void CRenderSystem::Print(const std::shared_ptr<CFont> &font, int x, int y, const std::string &text)
{
	if (m_dynamicTextBatchLookup.count(font) == 0)
	{
		m_dynamicTextBatches.push_back({ font, {} });
		m_dynamicTextBatchLookup.insert(std::make_pair(font, std::prev(m_dynamicTextBatches.end())));
	}

	m_dynamicTextBatchLookup[font]->textData.push({ x, y, text });
}

bool CRenderSystem::Update()
{
	UpdateTextureFilter();
	UpdateScene();
	UpdateUI();
	UpdateText();
	UpdateConsole();

	return false;
}

bool CRenderSystem::Render(float alpha)
{
	m_renderer->Clear();

	RenderScene();
	RenderUI();
	RenderText();
	RenderConsole();

	return false;
}

void CRenderSystem::RegisterTexture(CTexture &texture)
{
	if (find(m_textures.begin(), m_textures.end(), &texture) != m_textures.end())
		return;

	m_textures.push_back(&texture);
}

void CRenderSystem::UnregisterTexture(CTexture &texture)
{
	m_textures.remove(&texture);
}

void CRenderSystem::MoveConsoleUp(float distance)
{
	std::shared_ptr<CFont> font = m_consoleData.font;
	unsigned int columns = m_consoleData.columns;
	float top = m_consoleData.top;

	m_consoleTranslationMatrix[3][1] -= distance;

	m_vertexCount = columns * (static_cast<unsigned int>((m_consoleTranslationMatrix[3][1] - top) / font->GetCellHeight()) + 1) * 6;

	if (m_consoleTranslationMatrix[3][1] < top)
		m_consoleTranslationMatrix[3][1] = top;
}

void CRenderSystem::MoveConsoleDown(float distance)
{
	std::shared_ptr<CFont> font = m_consoleData.font;
	unsigned int columns = m_consoleData.columns;
	float bottom = m_consoleData.bottom;
	float top = m_consoleData.top;

	m_consoleTranslationMatrix[3][1] += distance;

	m_vertexCount = columns * (static_cast<unsigned int>((m_consoleTranslationMatrix[3][1] - top) / font->GetCellHeight()) + 1) * 6;

	if (m_consoleTranslationMatrix[3][1] > bottom)
		m_consoleTranslationMatrix[3][1] = bottom;
	if (m_consoleTranslationMatrix[3][1] == bottom)
		m_vertexCount = m_count / 4 * 6;
}

void CRenderSystem::ResetCursor()
{
	std::shared_ptr<CFont> font = m_consoleData.font;

	m_cursorTranslationMatrix[3][0] = static_cast<float>(font->GetWidth(">"));
}

void CRenderSystem::MoveCursorLeft()
{
	std::shared_ptr<CFont> font = m_consoleData.font;
	const std::string &input = *m_consoleData.input;

	m_cursorTranslationMatrix[3][0] -= static_cast<float>(font->GetWidth(std::string(1, input.back())));
}

void CRenderSystem::MoveCursorRight()
{
	std::shared_ptr<CFont> font = m_consoleData.font;
	const std::string &input = *m_consoleData.input;

	m_cursorTranslationMatrix[3][0] += static_cast<float>(font->GetWidth(std::string(1, input.back())));
}

void CRenderSystem::UpdateTextureFilter()
{
	if (!m_updateTextureFilter)
		return;

	for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
	{
		(*it)->Bind();
		(*it)->SetFilter(m_textureFilter);
	}

	m_updateTextureFilter = false;
}

void CRenderSystem::InitScene() {}

void CRenderSystem::UpdateScene()
{
	if (!m_updateScene)
		return;

	for (auto it = m_sceneEntityBatches.begin(); it != m_sceneEntityBatches.end(); ++it)
		delete (*it).first;

	m_sceneEntityBatches.clear();
	m_sceneEntityMeshLookup.clear();

	std::map<std::pair<std::vector<ECVertexComponent>, std::pair<bool, bool>>, std::list<std::pair<CVertexBuffer *, std::list<SceneEntityBatch>>>::iterator> sceneEntityBatchesLookup;
	std::map<std::pair<std::vector<ECVertexComponent>, std::pair<bool, bool>>, VertexDataBucket> vertexDataBuckets;
	const std::list<unsigned int> &sceneEntityHandles = m_game->GetSceneSystem().GetSceneEntityHandles();

	for (auto it = sceneEntityHandles.begin(); it != sceneEntityHandles.end(); ++it)
	{
		unsigned int handle = *it;
		SceneEntityData &sceneEntity = m_game->GetSceneSystem().GetSceneEntity(handle);
		CMesh *mesh = static_cast<CMesh *>(sceneEntity.object.get());
		bool instanced = mesh->IsInstanced();
		bool blended = mesh->GetMaterial()->GetBlendMode() != ECBlendMode::None;
		CGeometry *geometry = mesh->GetGeometry();
		std::pair<std::vector<ECVertexComponent>, std::pair<bool, bool>> criteria{ geometry->GetVertexComponents(), { instanced, blended } };

		if (sceneEntityBatchesLookup.count(criteria) == 0)
		{
			m_sceneEntityBatches.push_back(std::make_pair(nullptr, std::list<SceneEntityBatch>{}));
			sceneEntityBatchesLookup.insert(std::make_pair(criteria, std::prev(m_sceneEntityBatches.end())));
		}

		std::list<SceneEntityBatch> &sceneEntityBatches = sceneEntityBatchesLookup[criteria]->second;
		std::shared_ptr<CMaterial> material = mesh->GetMaterial();
		SceneEntityBatch &sceneEntityBatch = FindSceneEntityBatch(sceneEntityBatches, std::move(material), instanced);

		if (instanced)
		{
			std::list<unsigned int> &sceneEntityHandles = sceneEntityBatch.sceneEntityHandles;
			auto it = std::find_if(sceneEntityHandles.begin(), sceneEntityHandles.end(),
				[this, &sceneEntity](unsigned int handle)
				{
					return (static_cast<CMesh *>(m_game->GetSceneSystem().GetSceneEntity(handle).object.get())->GetID() == static_cast<CMesh *>(sceneEntity.object.get())->GetID());
				}
			);

			if (it == sceneEntityHandles.end())
				sceneEntityHandles.push_back(handle);
			else
				sceneEntityHandles.insert(it, handle);
		}
		else
			sceneEntityBatch.sceneEntityHandles.push_back(handle);

		if (m_sceneEntityMeshLookup.count(mesh->GetID()) == 0)
		{
			if (vertexDataBuckets.count(criteria) == 0)
				vertexDataBuckets.insert(std::make_pair(criteria, VertexDataBucket{ {}, 0 }));

			VertexDataBucket &vertexDataBucket = vertexDataBuckets[criteria];

			unsigned int start = vertexDataBucket.vertexCount;

			unsigned int count = geometry->GetVertexCount();

			vertexDataBucket.vertexData.insert(vertexDataBucket.vertexData.end(), geometry->GetVertexData().begin(), geometry->GetVertexData().end());

			vertexDataBucket.vertexCount += count;

			m_sceneEntityMeshLookup.insert(std::make_pair(mesh->GetID(), std::make_pair(start, count)));
		}
	}

	for (auto it = vertexDataBuckets.begin(); it != vertexDataBuckets.end(); ++it)
	{
		std::pair<std::vector<ECVertexComponent>, std::pair<bool, bool>> criteria = (*it).first;
		const std::vector<ECVertexComponent> &vertexComponents = criteria.first;
		const std::vector<float> &vertexData = (*it).second.vertexData;
		unsigned int vertexCount = (*it).second.vertexCount;
		CVertexBuffer *vertexBuffer = new CVertexBuffer(m_renderer, vertexComponents.data(), vertexComponents.size(), vertexData.data(), vertexData.size() * sizeof(float));
		vertexBuffer->AddIndexBuffer(nullptr, (vertexCount + 1) / 4 * 6 * sizeof(unsigned int), true);
		sceneEntityBatchesLookup[criteria]->first = vertexBuffer;
	}

	m_updateScene = false;
}

void CRenderSystem::RenderScene()
{
	if (m_sceneEntityBatches.empty())
		return;

	m_renderer->SetProjectionMatrix(m_game->GetCameraSystem().GetViewCamera().GetPerspectiveMatrix());
	m_renderer->SetViewMatrix(m_game->GetCameraSystem().GetViewCamera().GetViewMatrix());

	const glm::mat4 &projectionMatrix = m_renderer->GetProjectionMatrix();
	const glm::mat4 &viewMatrix = m_renderer->GetViewMatrix();

	m_renderer->SetCullFace(true);
	m_renderer->SetBlend(false);

	CShader *shader = nullptr;
	CTexture *texture = nullptr;
	ECMaterialProperties properties = ECMaterialProperties::None;
	bool doubleSided = false;
	bool transparent = false;
	ECBlendMode blendMode = ECBlendMode::None;

	for (auto it = m_sceneEntityBatches.begin(); it != m_sceneEntityBatches.end(); ++it)
	{
		CVertexBuffer *vertexBuffer = it->first;
		CIndexBuffer *indexBuffer = vertexBuffer->GetIndexBuffer();
		std::list<SceneEntityBatch> &sceneEntityBatches = it->second;

		vertexBuffer->Bind();
		indexBuffer->Bind();

		for (auto it = sceneEntityBatches.begin(); it != sceneEntityBatches.end(); ++it)
		{
			SceneEntityBatch &sceneEntityBatch = *it;

			if (sceneEntityBatch.material->GetShader() != shader)
			{
				shader = sceneEntityBatch.material->GetShader();
				shader->Use();
			}

			if (sceneEntityBatch.material->GetTexture() != texture)
			{
				texture = sceneEntityBatch.material->GetTexture();
				texture->Bind();
			}

			if (sceneEntityBatch.material->GetProperties() != properties)
			{
				properties = sceneEntityBatch.material->GetProperties();

				bool result = ((properties & ECMaterialProperties::DoubleSided) == ECMaterialProperties::DoubleSided);

				if (result != doubleSided)
				{
					doubleSided = result;

					m_renderer->SetCullFace(!doubleSided);
				}

				result = ((properties & ECMaterialProperties::Transparent) == ECMaterialProperties::Transparent);

				if (result != transparent)
				{
					transparent = result;

					m_renderer->SetBlend(transparent);
				}
			}

			if (transparent && sceneEntityBatch.material->GetBlendMode() != blendMode)
			{
				blendMode = sceneEntityBatch.material->GetBlendMode();

				m_renderer->SetBlendMode(blendMode);
			}

			if (sceneEntityBatch.instanced)
			{
				unsigned int *pIndex = static_cast<unsigned int *>(indexBuffer->Map("w"));

				unsigned int count = 0;

				std::list<unsigned int> &sceneEntityHandles = sceneEntityBatch.sceneEntityHandles;

				std::list<std::pair<unsigned int, unsigned int>> elements;

				for (auto it = sceneEntityHandles.begin(); it != sceneEntityHandles.end(); ++it)
				{
					unsigned int handle = *it;
					SceneEntityData &sceneEntity = m_game->GetSceneSystem().GetSceneEntity(handle);
					CMesh *mesh = static_cast<CMesh *>(sceneEntity.object.get());

					if (sceneEntity.visible && !mesh->IsHidden())
					{
						std::vector<unsigned int> indexData(mesh->GetIndexData().size());

						unsigned int start = m_sceneEntityMeshLookup[mesh->GetID()].first;
						std::transform(mesh->GetIndexData().begin(), mesh->GetIndexData().end(), indexData.begin(), [start](unsigned int index) { return start + index; });

						std::pair<unsigned int, unsigned int> element;
						element.first = count;
						element.second = mesh->GetIndexData().size();
						elements.push_back(element);

						count += element.second;

						memcpy(pIndex, indexData.data(), indexData.size() * sizeof(unsigned int));

						pIndex += indexData.size();
					}
				}

				indexBuffer->Unmap();

				auto iter = elements.begin();

				for (auto it = sceneEntityHandles.begin(); it != sceneEntityHandles.end(); ++it)
				{
					unsigned int handle = *it;
					SceneEntityData &sceneEntity = m_game->GetSceneSystem().GetSceneEntity(handle);
					CMesh *mesh = static_cast<CMesh *>(sceneEntity.object.get());

					if (sceneEntity.visible && !mesh->IsHidden())
					{
						std::pair<unsigned int, unsigned int> &element = *iter;

						mesh->SetUniforms();

						mesh->UpdateTransformationMatrix();

						shader->SetUniform("mvpMatrix", projectionMatrix * viewMatrix * mesh->GetTransformationMatrix() * (mesh->IsBillboard() ? glm::rotate(glm::radians(m_game->GetCameraSystem().GetViewCamera().GetAngleY()), glm::vec3(0.0f, 1.0f, 0.0f)) : glm::mat4(1.0f)));

						vertexBuffer->Draw(ECDrawMode::Triangles, element.first, element.second);

						sceneEntity.visible = sceneEntity.cameraDescendant;

						iter++;
					}
				}
			}
			else
			{
				unsigned int *pIndex = static_cast<unsigned int *>(indexBuffer->Map("w"));

				unsigned int count = 0;

				std::list<unsigned int> &sceneEntityHandles = sceneEntityBatch.sceneEntityHandles;

				std::list<std::pair<unsigned int, unsigned int>> elements;

				for (auto it = sceneEntityHandles.begin(); it != sceneEntityHandles.end(); ++it)
				{
					unsigned int handle = *it;
					SceneEntityData &sceneEntity = m_game->GetSceneSystem().GetSceneEntity(handle);
					CMesh *mesh = static_cast<CMesh *>(sceneEntity.object.get());

					if (sceneEntity.visible && !mesh->IsHidden())
					{
						std::vector<unsigned int> indexData(mesh->GetIndexData().size());

						unsigned int start = m_sceneEntityMeshLookup[mesh->GetID()].first;
						std::transform(mesh->GetIndexData().begin(), mesh->GetIndexData().end(), indexData.begin(), [start](unsigned int index) { return start + index; });

						std::pair<unsigned int, unsigned int> element;
						element.first = count;
						element.second = mesh->GetIndexData().size();
						elements.push_back(element);

						count += element.second;

						memcpy(pIndex, indexData.data(), indexData.size() * sizeof(unsigned int));

						pIndex += indexData.size();
					}
				}

				indexBuffer->Unmap();

				auto iter = elements.begin();

				for (auto it = sceneEntityHandles.begin(); it != sceneEntityHandles.end(); ++it)
				{
					unsigned int handle = *it;
					SceneEntityData &sceneEntity = m_game->GetSceneSystem().GetSceneEntity(handle);
					CMesh *mesh = static_cast<CMesh *>(sceneEntity.object.get());

					if (sceneEntity.visible && !mesh->IsHidden())
					{
						std::pair<unsigned int, unsigned int> &element = *iter;

						mesh->SetUniforms();

						mesh->UpdateTransformationMatrix();

						shader->SetUniform("mvpMatrix", projectionMatrix * viewMatrix * mesh->GetTransformationMatrix() * (mesh->IsBillboard() ? glm::rotate(glm::radians(m_game->GetCameraSystem().GetViewCamera().GetAngleY()), glm::vec3(0.0f, 1.0f, 0.0f)) : glm::mat4(1.0f)));

						vertexBuffer->Draw(ECDrawMode::Triangles, element.first, element.second);

						sceneEntity.visible = sceneEntity.cameraDescendant;

						iter++;
					}
				}
			}
		}
	}
}

void CRenderSystem::InitUI()
{
	m_uiElementShader = m_game->GetResourceManager().AcquireResource<CShader>("ui_element.shader");
}

void CRenderSystem::UpdateUI()
{
	if (!m_updateUI)
		return;

	m_uiElementBatches.clear();

	std::unordered_map<std::shared_ptr<CTexture>, std::list<UIElementBatch>::iterator> uiElementBatchLookup;
	std::vector<UIElementVertexData> vertices;
	const std::list<unsigned int> &uiElementHandles = m_game->GetUISystem().GetUIElementHandles();

	for (auto it = uiElementHandles.begin(); it != uiElementHandles.end(); ++it)
	{
		unsigned int handle = *it;
		UIElementData &uiElement = m_game->GetUISystem().GetUIElement(handle);

		if (uiElementBatchLookup.count(uiElement.texture) == 0)
		{
			m_uiElementBatches.push_back({ uiElement.texture, {} });
			uiElementBatchLookup.insert(std::make_pair(uiElement.texture, std::prev(m_uiElementBatches.end())));
		}

		uiElementBatchLookup[uiElement.texture]->uiElementHandles.push_back(handle);

		float x1 = static_cast<float>(uiElement.absoluteRect.x);
		float y1 = static_cast<float>(uiElement.absoluteRect.y);
		float x2 = x1 + uiElement.absoluteRect.w;
		float y2 = y1 + uiElement.absoluteRect.h;
		float u1 = uiElement.textureRect.x / static_cast<float>(uiElement.texture->GetWidth());
		float v1 = uiElement.textureRect.y / static_cast<float>(uiElement.texture->GetHeight());
		float u2 = u1 + ((uiElement.properties & ECUIElementProperties::RepeatTextureHorizontally) == ECUIElementProperties::RepeatTextureHorizontally ? uiElement.absoluteRect.w : uiElement.textureRect.w) / static_cast<float>(uiElement.texture->GetWidth());
		float v2 = v1 + ((uiElement.properties & ECUIElementProperties::RepeatTextureVertically) == ECUIElementProperties::RepeatTextureVertically ? uiElement.absoluteRect.h : uiElement.textureRect.h) / static_cast<float>(uiElement.texture->GetHeight());

		vertices.push_back({ x1, y1, u1, v1 });
		vertices.push_back({ x1, y2, u1, v2 });
		vertices.push_back({ x2, y1, u2, v1 });
		vertices.push_back({ x2, y2, u2, v2 });
	}

	ECVertexComponent components[2] = { ECVertexComponent::Position2D, ECVertexComponent::TexCoord2D };

	if (m_uiElementVertexBuffer != nullptr)
		delete m_uiElementVertexBuffer;

	m_uiElementVertexBuffer = new CVertexBuffer(m_renderer, components, 2, vertices.data(), vertices.size() * sizeof(UIElementVertexData));
	m_uiElementVertexBuffer->AddIndexBuffer(nullptr, vertices.size() / 4 * 6 * sizeof(unsigned int), true);

	m_updateUI = false;
}

void CRenderSystem::RenderUI()
{
	if (m_uiElementBatches.empty())
		return;

	CIndexBuffer *indexBuffer = m_uiElementVertexBuffer->GetIndexBuffer();

	m_uiElementShader->Use();
	m_uiElementVertexBuffer->Bind();
	indexBuffer->Bind();
	m_renderer->SetDepthTest(false);
	m_renderer->SetBlend(true);
	m_renderer->SetBlendMode(ECBlendMode::Interpolative);
	m_uiElementShader->SetUniform("mvpMatrix", m_renderer->GetOrthoMatrix());

	for (auto it = m_uiElementBatches.begin(); it != m_uiElementBatches.end(); ++it)
	{
		UIElementBatch &uiElementBatch = *it;
		std::shared_ptr<CTexture> texture = uiElementBatch.texture;
		std::list<unsigned int> &uiElementHandles = uiElementBatch.uiElementHandles;

		unsigned int count = 0;

		unsigned int *pIndex = static_cast<unsigned int *>(indexBuffer->Map("w"));

		for (auto it = uiElementHandles.begin(); it != uiElementHandles.end(); ++it)
		{
			unsigned int handle = *it;
			UIElementData &uiElement = m_game->GetUISystem().GetUIElement(handle);

			if (!uiElement.hidden)
			{
				unsigned int start = uiElement.index * 4;

				pIndex[0] = start + 0;
				pIndex[1] = start + 1;
				pIndex[2] = start + 2;
				pIndex[3] = start + 2;
				pIndex[4] = start + 1;
				pIndex[5] = start + 3;

				pIndex += 6;

				count += 6;
			}
		}

		indexBuffer->Unmap();

		if (count != 0)
		{
			texture->Bind();
			m_uiElementVertexBuffer->Draw(ECDrawMode::Triangles, 0, count);
		}
	}

	m_renderer->SetBlend(false);
	m_renderer->SetDepthTest(true);
}

void CRenderSystem::InitText()
{
	m_textShader = m_game->GetResourceManager().AcquireResource<CShader>("text.shader");

	ECVertexComponent components[2] = { ECVertexComponent::Position2D, ECVertexComponent::TexCoord2D };

	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < 256; i++)
	{
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 3);
	}

	m_textVertexBuffer = new CVertexBuffer(m_renderer, components, 2, nullptr, 256 * 4 * sizeof(TextVertexData), true);
	m_textVertexBuffer->AddIndexBuffer(indices.data(), indices.size() * sizeof(unsigned int));
}

void CRenderSystem::UpdateText() {}

void CRenderSystem::RenderText()
{
	if (m_dynamicTextBatches.empty())
		return;

	m_textShader->Use();
	m_textVertexBuffer->Bind();
	m_textVertexBuffer->GetIndexBuffer()->Bind();
	m_renderer->SetDepthTest(false);
	m_renderer->SetBlend(true);
	m_renderer->SetBlendMode(ECBlendMode::Interpolative);
	m_textShader->SetUniform("mvpMatrix", m_renderer->GetOrthoMatrix());

	int scale = m_game->GetUISystem().GetScale();

	for (auto it = m_dynamicTextBatches.begin(); it != m_dynamicTextBatches.end(); ++it)
	{
		DynamicTextBatch &dynamicTextBatch = *it;
		std::shared_ptr<CFont> font = dynamicTextBatch.font;
		std::queue<TextData> &textData = dynamicTextBatch.textData;

		unsigned int cellWidth = font->GetCellWidth();
		unsigned int cellHeight = font->GetCellHeight();
		unsigned int rowPitch = font->GetRowPitch();
		float columnFactor = font->GetColumnFactor();
		float rowFactor = font->GetRowFactor();
		char base = font->GetBase();

		unsigned int count = 0;

		TextVertexData *pVertex = static_cast<TextVertexData *>(m_textVertexBuffer->Map("w"));

		while (!textData.empty())
		{
			TextData text = textData.front();
			textData.pop();

			const char *c_str = text.text.c_str();
			char c;

			if (font->GetBGColor().a != 0)
			{
				glm::mat4 matrix = glm::translate(glm::vec3(text.x, text.y, 0.0f)) * glm::scale(glm::vec3(font->GetWidth(text.text) / static_cast<float>(cellWidth) * scale, scale, 1.0f));

				glm::vec4 min = matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				glm::vec4 max = matrix * glm::vec4(static_cast<float>(cellWidth), static_cast<float>(cellHeight), 0.0f, 1.0f);

				unsigned int row = (' ' - base) / rowPitch;
				unsigned int column = (' ' - base) - row * rowPitch;
				float u1 = column * columnFactor;
				float v1 = row * rowFactor;
				float u2 = u1 + columnFactor;
				float v2 = v1 + rowFactor;

				TextVertexData minVertex = { min.x, min.y, u1, v1 };
				TextVertexData maxVertex = { max.x, max.y, u2, v2 };

				pVertex[0] = { minVertex.position.x, minVertex.position.y, minVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[1] = { minVertex.position.x, maxVertex.position.y, minVertex.texCoord.u, maxVertex.texCoord.v };
				pVertex[2] = { maxVertex.position.x, minVertex.position.y, maxVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[3] = { maxVertex.position.x, maxVertex.position.y, maxVertex.texCoord.u, maxVertex.texCoord.v };

				pVertex += 4;

				count += 6;
			}

			glm::mat4 matrix = glm::translate(glm::vec3(text.x, text.y, 0.0f)) * glm::scale(glm::vec3(scale, scale, 1.0f));

			glm::vec4 min = matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			glm::vec4 max = matrix * glm::vec4(static_cast<float>(cellWidth), static_cast<float>(cellHeight), 0.0f, 1.0f);

			while ((c = *(c_str++)) != 0)
			{
				unsigned int row = (c - base) / rowPitch;
				unsigned int column = (c - base) - row * rowPitch;
				float u1 = column * columnFactor;
				float v1 = row * rowFactor;
				float u2 = u1 + columnFactor;
				float v2 = v1 + rowFactor;

				TextVertexData minVertex = { min.x, min.y, u1, v1 };
				TextVertexData maxVertex = { max.x, max.y, u2, v2 };

				pVertex[0] = { minVertex.position.x, minVertex.position.y, minVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[1] = { minVertex.position.x, maxVertex.position.y, minVertex.texCoord.u, maxVertex.texCoord.v };
				pVertex[2] = { maxVertex.position.x, minVertex.position.y, maxVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[3] = { maxVertex.position.x, maxVertex.position.y, maxVertex.texCoord.u, maxVertex.texCoord.v };

				pVertex += 4;

				count += 6;

				min.x += font->GetWidth(c) * scale;
				max.x += font->GetWidth(c) * scale;
			}
		}

		m_textVertexBuffer->Unmap();

		if (count != 0)
		{
			font->GetTexture()->Bind();
			m_textShader->SetUniform("fgColor", glm::vec4(font->GetFGColor()) / 255.0f);
			m_textShader->SetUniform("bgColor", glm::vec4(font->GetBGColor()) / 255.0f);
			m_textVertexBuffer->Draw(ECDrawMode::Triangles, 0, count);
		}
	}

	m_renderer->SetBlend(false);
	m_renderer->SetDepthTest(true);
}

void CRenderSystem::InitConsole()
{
	m_consoleShader = m_game->GetResourceManager().AcquireResource<CShader>("text.shader");

	std::shared_ptr<CFont> font = m_consoleData.font;
	unsigned int cellWidth = font->GetCellWidth();
	unsigned int cellHeight = font->GetCellHeight();
	unsigned int rowPitch = font->GetRowPitch();
	float columnFactor = font->GetColumnFactor();
	float rowFactor = font->GetRowFactor();
	char base = font->GetBase();
	unsigned int columns = m_consoleData.columns;
	unsigned int rows = m_consoleData.rows;
	const std::list<std::string> &lines = *m_consoleData.lines;
	const std::string &input = *m_consoleData.input;
	float bottom = m_consoleData.bottom;

	m_consoleTranslationMatrix = glm::translate<float>(glm::vec3((m_renderer->GetViewportWidth() - columns * (cellWidth - ((cellWidth % 2) == 0 ? 0 : 1))) / 2, 0.0f, 0.0f));
	m_cursorTranslationMatrix = glm::translate<float>(glm::vec3((m_renderer->GetViewportWidth() - columns * (cellWidth - ((cellWidth % 2) == 0 ? 0 : 1))) / 2 + static_cast<float>(font->GetWidth(">" + input.substr(input.length() < columns - 2 ? 0 : input.length() - (columns - 2)))), bottom - cellHeight, 0.0f));

	m_count = columns * rows * 4;
	m_size = m_count;

	std::vector<ConsoleVertexData> vertices(m_size + 8);

	m_size *= sizeof(ConsoleVertexData);

	std::list<std::string>::const_iterator it = *m_consoleData.bottomLine;
	unsigned int size = lines.size();
	unsigned int k = 0;

	for (unsigned int i = 0; i < rows; i++)
	{
		std::string line;
		unsigned int length;

		if (i > 0 && i <= size)
		{
			line = *(it++);
			length = line.length() - (line.back() == '\n' ? 1 : 0);
		}
		else
			length = input.length();

		float x = 0.0f;
		float y = -static_cast<float>((i + 1) * cellHeight);

		for (unsigned int j = 0; j < columns; j++, k += 4)
		{
			char c;

			if (i > 0 && i <= size && j < length)
				c = line[j];
			else if (i == 0 && j == 0)
				c = '>';
			else if (i == 0 && j <= length && j < columns - 1)
				c = input[(length < columns - 1 ? 0 : length - (columns - 2)) + j - 1];
			else
				c = ' ';

			unsigned int row = (c - base) / rowPitch;
			unsigned int column = (c - base) - row * rowPitch;
			float u1 = column * columnFactor;
			float v1 = row * rowFactor;
			float u2 = u1 + columnFactor;
			float v2 = v1 + rowFactor;

			ConsoleVertexData minVertex = { x, y, u1, v1 };
			ConsoleVertexData maxVertex = { x + cellWidth, y + cellHeight, u2, v2 };

			vertices[k + 0] = { minVertex.position.x, minVertex.position.y, minVertex.texCoord.u, minVertex.texCoord.v };
			vertices[k + 1] = { minVertex.position.x, maxVertex.position.y, minVertex.texCoord.u, maxVertex.texCoord.v };
			vertices[k + 2] = { maxVertex.position.x, minVertex.position.y, maxVertex.texCoord.u, minVertex.texCoord.v };
			vertices[k + 3] = { maxVertex.position.x, maxVertex.position.y, maxVertex.texCoord.u, maxVertex.texCoord.v };

			x += font->GetWidth(std::string(1, c));
		}
	}

	unsigned int row = ('|' - base) / rowPitch;
	unsigned int column = ('|' - base) - row * rowPitch;
	float u1 = column * columnFactor;
	float v1 = row * rowFactor;
	float u2 = u1 + columnFactor;
	float v2 = v1 + rowFactor;

	ConsoleVertexData minVertex = { 0.0f, 0.0f, u1, v1 };
	ConsoleVertexData maxVertex = { static_cast<float>(cellWidth), static_cast<float>(cellHeight), u2, v2 };

	vertices[k + 0] = { minVertex.position.x, minVertex.position.y, minVertex.texCoord.u, minVertex.texCoord.v };
	vertices[k + 1] = { minVertex.position.x, maxVertex.position.y, minVertex.texCoord.u, maxVertex.texCoord.v };
	vertices[k + 2] = { maxVertex.position.x, minVertex.position.y, maxVertex.texCoord.u, minVertex.texCoord.v };
	vertices[k + 3] = { maxVertex.position.x, maxVertex.position.y, maxVertex.texCoord.u, maxVertex.texCoord.v };

	k += 4;

	row = (' ' - base) / rowPitch;
	column = (' ' - base) - row * rowPitch;
	u1 = column * columnFactor;
	v1 = row * rowFactor;
	u2 = u1 + columnFactor;
	v2 = v1 + rowFactor;

	minVertex = { 0.0f, -static_cast<float>(rows * cellHeight), u1, v1 };
	maxVertex = { static_cast<float>(columns * cellWidth), 0.0f, u2, v2 };

	vertices[k + 0] = { minVertex.position.x, minVertex.position.y, minVertex.texCoord.u, minVertex.texCoord.v };
	vertices[k + 1] = { minVertex.position.x, maxVertex.position.y, minVertex.texCoord.u, maxVertex.texCoord.v };
	vertices[k + 2] = { maxVertex.position.x, minVertex.position.y, maxVertex.texCoord.u, minVertex.texCoord.v };
	vertices[k + 3] = { maxVertex.position.x, maxVertex.position.y, maxVertex.texCoord.u, maxVertex.texCoord.v };

	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < columns * rows + 2; i++)
	{
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 3);
	}

	if (m_consoleVertexBuffer != nullptr)
		delete m_consoleVertexBuffer;

	ECVertexComponent components[2] = { ECVertexComponent::Position2D, ECVertexComponent::TexCoord2D };

	m_consoleVertexBuffer = new CVertexBuffer(m_renderer, components, 2, vertices.data(), vertices.size() * sizeof(ConsoleVertexData), true);
	m_consoleVertexBuffer->AddIndexBuffer(indices.data(), indices.size() * sizeof(unsigned int));

	m_consoleData.initialize = false;

	m_updateConsole = false;
}

void CRenderSystem::UpdateConsole()
{
	if (!m_updateConsole)
		return;

	if (m_consoleData.initialize)
	{
		InitConsole();

		return;
	}

	std::shared_ptr<CFont> font = m_consoleData.font;
	unsigned int columns = m_consoleData.columns;
	unsigned int rows = m_consoleData.rows;
	const std::string &input = *m_consoleData.input;
	bool updateFirstRow = m_consoleData.updateFirstRow;
	bool updateSecondRow = m_consoleData.updateSecondRow;
	bool updateLastRow = m_consoleData.updateLastRow;
	unsigned int rowUpdateCount = m_consoleData.rowUpdateCount;
	unsigned int charUpdateCount = m_consoleData.charUpdateCount;
	bool carriageReturn = m_consoleData.carriageReturn;
	bool backspace = m_consoleData.backspace;

	ConsoleVertexData *pVertex = static_cast<ConsoleVertexData *>(m_consoleVertexBuffer->Map("rw"));

	if (updateFirstRow)
	{
		if (carriageReturn && !backspace && input.length() > columns - 2 && charUpdateCount < columns - 2)
		{
			memmove(&pVertex[4], &pVertex[4 + charUpdateCount * 4], (columns - 3 - (charUpdateCount - 1)) * 4 * sizeof(ConsoleVertexData));

			float offset = pVertex[4].position.x - static_cast<float>(font->GetWidth(">"));

			for (unsigned int i = 4; i < (columns - 2 - (charUpdateCount - 1)) * 4; i++)
				pVertex[i].position.x -= offset;
		}
		else if (backspace && input.length() + 1 > columns - 2)
		{
			memmove(&pVertex[8], &pVertex[4], (columns - 3) * 4 * sizeof(ConsoleVertexData));

			float offset = static_cast<float>(font->GetWidth(std::string(1, input[input.length() - (columns - 2)])));

			for (unsigned int i = 8; i < (columns - 2 - (charUpdateCount - 1)) * 4; i++)
				pVertex[i].position.x += offset;
		}

		UpdateConsoleRows(pVertex, 1);
	}

	if (updateSecondRow)
	{
		if (rowUpdateCount == 0)
			rowUpdateCount = 1;

		if (rowUpdateCount < rows - 1)
		{
			memmove(&pVertex[(rowUpdateCount + 1) * columns * 4], &pVertex[columns * 4], m_size - (rowUpdateCount + 1) * columns * 4 * sizeof(ConsoleVertexData));

			float offset = static_cast<float>(rowUpdateCount * font->GetCellHeight());

			for (unsigned int i = (rowUpdateCount + 1) * columns * 4; i < m_count; i++)
				pVertex[i].position.y -= offset;
		}
		else
			rowUpdateCount = rows - 1;

		UpdateConsoleRows(pVertex, 2, rowUpdateCount + 1);

		updateLastRow = false;
	}

	if (updateLastRow)
	{
		memmove(&pVertex[columns * 4], &pVertex[2 * columns * 4], m_size - 2 * columns * 4 * sizeof(ConsoleVertexData));

		float offset = static_cast<float>(font->GetCellHeight());

		for (unsigned int i = columns * 4; i < m_count - columns * 4; i++)
			pVertex[i].position.y += offset;

		UpdateConsoleRows(pVertex, rows);
	}

	m_consoleVertexBuffer->Unmap();

	m_consoleData.updateFirstRow = false;
	m_consoleData.updateSecondRow = false;
	m_consoleData.updateLastRow = false;
	m_consoleData.rowUpdateCount = 0;
	m_consoleData.charUpdateCount = 0;
	m_consoleData.carriageReturn = false;
	m_consoleData.backspace = false;

	m_updateConsole = false;
}

void CRenderSystem::RenderConsole()
{
	if (m_game->GetConsole().IsClosed())
		return;

	std::shared_ptr<CFont> font = m_consoleData.font;
	const glm::uvec4 &fgColor = *m_consoleData.fgColor;
	const glm::uvec4 &bgColor = *m_consoleData.bgColor;
	bool showCursor = m_consoleData.showCursor;

	const glm::mat4 &orthoMatrix = m_renderer->GetOrthoMatrix();

	m_consoleShader->Use();
	m_consoleVertexBuffer->Bind();
	m_consoleVertexBuffer->GetIndexBuffer()->Bind();
	m_renderer->SetDepthTest(false);
	m_renderer->SetBlend(true);
	m_renderer->SetBlendMode(ECBlendMode::Interpolative);
	font->GetTexture()->Bind();
	m_consoleShader->SetUniform("fgColor", glm::vec4(fgColor) / 255.0f);
	m_consoleShader->SetUniform("bgColor", glm::vec4(bgColor) / 255.0f);
	m_consoleShader->SetUniform("mvpMatrix", orthoMatrix * m_consoleTranslationMatrix);

	m_consoleVertexBuffer->Draw(ECDrawMode::Triangles, m_count / 4 * 6 + 6, 6);

	m_consoleShader->SetUniform("bgColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	m_consoleVertexBuffer->Draw(ECDrawMode::Triangles, 0, m_vertexCount);

	if (showCursor)
	{
		m_consoleShader->SetUniform("mvpMatrix", orthoMatrix * m_cursorTranslationMatrix);

		m_consoleVertexBuffer->Draw(ECDrawMode::Triangles, m_count / 4 * 6, 6);
	}

	m_renderer->SetBlend(false);
	m_renderer->SetDepthTest(true);
}

SceneEntityBatch &CRenderSystem::FindSceneEntityBatch(std::list<SceneEntityBatch> &sceneEntityBatches, std::shared_ptr<CMaterial> material, bool instanced)
{
	std::list<SceneEntityBatch>::iterator it;

	if (!sceneEntityBatches.empty())
	{
		it = std::find_if(sceneEntityBatches.begin(), sceneEntityBatches.end(),
			[&material](const SceneEntityBatch &sceneEntityBatch)
			{
				return (sceneEntityBatch.material->GetShader() == material->GetShader());
			}
		);
		it = std::find_if(it, sceneEntityBatches.end(),
			[&material](const SceneEntityBatch &sceneEntityBatch)
			{
				return (sceneEntityBatch.material->GetShader() != material->GetShader() || sceneEntityBatch.material->GetTexture() == material->GetTexture());
			}
		);

		if (it == sceneEntityBatches.end())
		{
			sceneEntityBatches.push_back({ std::move(material), instanced, {} });
			it = std::prev(sceneEntityBatches.end());
		}
		else if (it->material->GetShader() != material->GetShader())
			it = sceneEntityBatches.insert(it, { std::move(material), instanced, {} });
		else
		{
			it = std::find_if(it, sceneEntityBatches.end(),
				[&material](const SceneEntityBatch &sceneEntityBatch)
				{
					return (sceneEntityBatch.material->GetTexture() != material->GetTexture() || sceneEntityBatch.material->GetProperties() >= material->GetProperties());
				}
			);

			if (it == sceneEntityBatches.end())
			{
				sceneEntityBatches.push_back({ std::move(material), instanced, {} });
				it = std::prev(sceneEntityBatches.end());
			}
			else if (it->material->GetTexture() != material->GetTexture())
				it = sceneEntityBatches.insert(it, { std::move(material), instanced, {} });
			else
			{
				it = std::find_if(it, sceneEntityBatches.end(),
					[&material](const SceneEntityBatch &sceneEntityBatch)
					{
						return (sceneEntityBatch.material->GetProperties() != material->GetProperties() || sceneEntityBatch.material->GetBlendMode() == material->GetBlendMode());
					}
				);

				if (it == sceneEntityBatches.end())
				{
					sceneEntityBatches.push_back({ std::move(material), instanced, {} });
					it = std::prev(sceneEntityBatches.end());
				}
				else if (it->material->GetProperties() != material->GetProperties())
					it = sceneEntityBatches.insert(it, { std::move(material), instanced, {} });
			}
		}
	}
	else
	{
		sceneEntityBatches.push_back({ std::move(material), instanced, {} });
		it = std::prev(sceneEntityBatches.end());
	}

	return *it;
}

void CRenderSystem::UpdateConsoleRows(ConsoleVertexData *pVertices, unsigned int startRow, unsigned int endRow)
{
	std::shared_ptr<CFont> font = m_consoleData.font;
	unsigned int cellWidth = font->GetCellWidth();
	unsigned int cellHeight = font->GetCellHeight();
	unsigned int rowPitch = font->GetRowPitch();
	float columnFactor = font->GetColumnFactor();
	float rowFactor = font->GetRowFactor();
	char base = font->GetBase();
	unsigned int columns = m_consoleData.columns;
	unsigned int rows = m_consoleData.rows;
	const std::string &input = *m_consoleData.input;
	unsigned int charUpdateCount = m_consoleData.charUpdateCount;
	bool carriageReturn = m_consoleData.carriageReturn;
	bool backspace = m_consoleData.backspace;

	if (startRow != 1)
	{
		std::list<std::string>::const_iterator it;

		if (startRow == 2)
			it = *m_consoleData.bottomLine;
		else
		{
			it = *m_consoleData.topLine;
			endRow = rows;
		}

		ConsoleVertexData *pVertex = &pVertices[(startRow - 1) * columns * 4];

		for (unsigned int i = startRow - 1; i < endRow; i++)
		{
			std::string line = *(it++);
			unsigned int length = line.length() - (line.back() == '\n' ? 1 : 0);

			float x = 0.0f;
			float y = -static_cast<float>((i + 1) * cellHeight);

			for (unsigned int j = 0; j < columns; j++, pVertex += 4)
			{
				char c;

				if (j < length)
					c = line[j];
				else
					c = ' ';

				unsigned int row = (c - base) / rowPitch;
				unsigned int column = (c - base) - row * rowPitch;
				float u1 = column * columnFactor;
				float v1 = row * rowFactor;
				float u2 = u1 + columnFactor;
				float v2 = v1 + rowFactor;

				ConsoleVertexData minVertex = { x, y, u1, v1 };
				ConsoleVertexData maxVertex = { x + cellWidth, y + cellHeight, u2, v2 };

				pVertex[0] = { minVertex.position.x, minVertex.position.y, minVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[1] = { minVertex.position.x, maxVertex.position.y, minVertex.texCoord.u, maxVertex.texCoord.v };
				pVertex[2] = { maxVertex.position.x, minVertex.position.y, maxVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[3] = { maxVertex.position.x, maxVertex.position.y, maxVertex.texCoord.u, maxVertex.texCoord.v };

				x += font->GetWidth(std::string(1, c));
			}
		}
	}
	else
	{
		unsigned int length = input.length();

		if (charUpdateCount > columns - 2)
			charUpdateCount = columns - 2;

		float x = m_cursorTranslationMatrix[3][0] - font->GetWidth(input.substr(length - charUpdateCount));
		float y = -static_cast<float>(cellHeight);

		while (charUpdateCount > 0)
		{
			unsigned int column;

			if (length <= columns - 2)
				column = length - charUpdateCount + 1;
			else
				column = columns - charUpdateCount - 1;

			char c = input[length - charUpdateCount];

			ConsoleVertexData *pVertex = &pVertices[column * 4];

			unsigned int row = (c - base) / rowPitch;
			column = (c - base) - row * rowPitch;
			float u1 = column * columnFactor;
			float v1 = row * rowFactor;
			float u2 = u1 + columnFactor;
			float v2 = v1 + rowFactor;

			ConsoleVertexData minVertex = { x, y, u1, v1 };
			ConsoleVertexData maxVertex = { x + cellWidth, y + cellHeight, u2, v2 };

			pVertex[0] = { minVertex.position.x, minVertex.position.y, minVertex.texCoord.u, minVertex.texCoord.v };
			pVertex[1] = { minVertex.position.x, maxVertex.position.y, minVertex.texCoord.u, maxVertex.texCoord.v };
			pVertex[2] = { maxVertex.position.x, minVertex.position.y, maxVertex.texCoord.u, minVertex.texCoord.v };
			pVertex[3] = { maxVertex.position.x, maxVertex.position.y, maxVertex.texCoord.u, maxVertex.texCoord.v };

			x += font->GetWidth(std::string(1, c));

			charUpdateCount--;
		}

		if (carriageReturn)
		{
			ConsoleVertexData *pVertex = &pVertices[4];

			unsigned int row = (' ' - base) / rowPitch;
			unsigned int column = (' ' - base) - row * rowPitch;
			float u1 = column * columnFactor;
			float v1 = row * rowFactor;
			float u2 = u1 + columnFactor;
			float v2 = v1 + rowFactor;

			ConsoleVertexData minVertex = { x, y, u1, v1 };
			ConsoleVertexData maxVertex = { x + cellWidth, y + cellHeight, u2, v2 };

			column = 1;
			unsigned int width = font->GetWidth(std::string(1, ' '));

			while (column < columns)
			{
				pVertex[0] = { minVertex.position.x, minVertex.position.y, minVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[1] = { minVertex.position.x, maxVertex.position.y, minVertex.texCoord.u, maxVertex.texCoord.v };
				pVertex[2] = { maxVertex.position.x, minVertex.position.y, maxVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[3] = { maxVertex.position.x, maxVertex.position.y, maxVertex.texCoord.u, maxVertex.texCoord.v };

				pVertex += 4;

				minVertex.position.x += width;
				maxVertex.position.x += width;

				column++;
			}
		}
		else if (backspace)
		{
			if (length + 1 > columns - 2)
			{
				x = static_cast<float>(font->GetWidth(">"));

				char c = input[length - (columns - 2)];

				ConsoleVertexData *pVertex = &pVertices[4];

				unsigned int row = (c - base) / rowPitch;
				unsigned int column = (c - base) - row * rowPitch;
				float u1 = column * columnFactor;
				float v1 = row * rowFactor;
				float u2 = u1 + columnFactor;
				float v2 = v1 + rowFactor;

				ConsoleVertexData minVertex = { x, y, u1, v1 };
				ConsoleVertexData maxVertex = { x + cellWidth, y + cellHeight, u2, v2 };

				pVertex[0] = { minVertex.position.x, minVertex.position.y, minVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[1] = { minVertex.position.x, maxVertex.position.y, minVertex.texCoord.u, maxVertex.texCoord.v };
				pVertex[2] = { maxVertex.position.x, minVertex.position.y, maxVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[3] = { maxVertex.position.x, maxVertex.position.y, maxVertex.texCoord.u, maxVertex.texCoord.v };
			}
			else
			{
				ConsoleVertexData *pVertex = &pVertices[(input.length() + 1) * 4];

				unsigned int row = (' ' - base) / rowPitch;
				unsigned int column = (' ' - base) - row * rowPitch;
				float u1 = column * columnFactor;
				float v1 = row * rowFactor;
				float u2 = u1 + columnFactor;
				float v2 = v1 + rowFactor;

				ConsoleVertexData minVertex = { x, y, u1, v1 };
				ConsoleVertexData maxVertex = { x + cellWidth, y + cellHeight, u2, v2 };

				pVertex[0] = { minVertex.position.x, minVertex.position.y, minVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[1] = { minVertex.position.x, maxVertex.position.y, minVertex.texCoord.u, maxVertex.texCoord.v };
				pVertex[2] = { maxVertex.position.x, minVertex.position.y, maxVertex.texCoord.u, minVertex.texCoord.v };
				pVertex[3] = { maxVertex.position.x, maxVertex.position.y, maxVertex.texCoord.u, maxVertex.texCoord.v };
			}
		}
	}
}