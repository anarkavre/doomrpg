#ifndef __CRENDERSYSTEM_H__
#define __CRENDERSYSTEM_H__

#include <list>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

#include "glm/glm.hpp"

#include "Interface/IRenderHandler.h"
#include "Interface/ISystem.h"
#include "Material/CMaterial.h"

class CFont;
class CShader;
class CTexture;
class CVertexBuffer;
class IRenderer;

struct SceneEntityBatch
{
	std::shared_ptr<CMaterial> material;
	bool instanced;
	std::list<unsigned int> sceneEntityHandles;
};

struct UIElementBatch
{
	std::shared_ptr<CTexture> texture;
	std::list<unsigned int> uiElementHandles;
};

struct TextData
{
	int x, y;
	std::string text;
};

struct DynamicTextBatch
{
	std::shared_ptr<CFont> font;
	std::queue<TextData> textData;
};

struct ConsoleData
{
	bool initialize;
	std::shared_ptr<CFont> font;
	const glm::uvec4 *fgColor;
	const glm::uvec4 *bgColor;
	unsigned int columns;
	unsigned int rows;
	const std::list<std::string> *lines;
	const std::string *input;
	const std::list<std::string>::const_iterator *bottomLine;
	const std::list<std::string>::const_iterator *topLine;
	float bottom;
	float top;
	bool updateFirstRow;
	bool updateSecondRow;
	bool updateLastRow;
	unsigned int rowUpdateCount;
	unsigned int charUpdateCount;
	bool carriageReturn;
	bool backspace;
	bool showCursor;
};

struct ConsoleVertexData;

class CRenderSystem : public ISystem, public IRenderHandler
{
public:
	CRenderSystem(CGame *game = nullptr) { Initialize(game); }
	~CRenderSystem();

	bool Initialize(CGame *game);
	void Print(const std::shared_ptr<CFont> &font, int x, int y, const std::string &text);
	bool Update();
	bool Render(float alpha);

	void RegisterTexture(CTexture &texture);
	void UnregisterTexture(CTexture &texture);

	void UpdateScene(bool updateScene) { m_updateScene = updateScene; }
	void UpdateUI(bool updateUI) { m_updateUI = updateUI; }
	void UpdateText(bool updateText) { m_updateText = updateText; }
	void UpdateConsole(bool updateConsole) { m_updateConsole = updateConsole; }

	void MoveConsoleUp(float distance);
	void MoveConsoleDown(float distance);
	void ResetCursor();
	void MoveCursorLeft();
	void MoveCursorRight();

	ECTextureFilter GetTextureFilter() const { return m_textureFilter; }
	void SetTextureFilter(ECTextureFilter textureFilter) { m_textureFilter = textureFilter; m_updateTextureFilter = true; }
	ConsoleData &GetConsoleData() { return m_consoleData; }

private:
	void UpdateTextureFilter();
	void InitScene();
	void UpdateScene();
	void RenderScene();
	void InitUI();
	void UpdateUI();
	void RenderUI();
	void InitText();
	void UpdateText();
	void RenderText();
	void InitConsole();
	void UpdateConsole();
	void RenderConsole();

	SceneEntityBatch &FindSceneEntityBatch(std::list<SceneEntityBatch> &sceneEntityBatches, std::shared_ptr<CMaterial> material, bool instanced);
	void UpdateConsoleRows(ConsoleVertexData *pVertices, unsigned int startRow, unsigned int endRow = 0);

	IRenderer *m_renderer{nullptr};
	bool m_updateTextureFilter{false}, m_updateScene{false}, m_updateUI{false}, m_updateText{false}, m_updateConsole{false};
	ECTextureFilter m_textureFilter{ECTextureFilter::Point};
	std::list<CTexture *> m_textures;
	std::list<std::pair<CVertexBuffer *, std::list<SceneEntityBatch>>> m_sceneEntityBatches;
	std::unordered_map<unsigned int, std::pair<unsigned int, unsigned int>> m_sceneEntityMeshLookup;
	std::shared_ptr<CShader> m_uiElementShader, m_textShader, m_consoleShader;
	CVertexBuffer *m_uiElementVertexBuffer{nullptr}, *m_textVertexBuffer{nullptr}, * m_consoleVertexBuffer{nullptr};
	std::unordered_map<std::shared_ptr<CFont>, std::list<DynamicTextBatch>::iterator> m_dynamicTextBatchLookup;
	std::list<UIElementBatch> m_uiElementBatches;
	std::list<DynamicTextBatch> m_dynamicTextBatches;
	glm::mat4 m_consoleTranslationMatrix{0.0f}, m_cursorTranslationMatrix{0.0f};
	unsigned int m_count{0}, m_size{0}, m_vertexCount{0};
	ConsoleData m_consoleData;
};

#endif