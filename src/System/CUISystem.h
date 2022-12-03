#ifndef __CUSERINTERFACE_H__
#define __CUSERINTERFACE_H__

#include <list>
#include <memory>
#include <string>

#include "Interface/ISystem.h"
#include "Pool/CPool.h"
#include "UI/UIElement.h"

class CFont;
class CTexture;

struct UIElementData
{
	UIElementRect absoluteRect;
	UIElementRect relativeRect;
	ECUIElementHorizontalAlignment xAlignment;
	ECUIElementVerticalAlignment yAlignment;
	std::shared_ptr<CTexture> texture;
	UIElementRect textureRect;
	ECUIElementProperties properties;
	bool hidden;
	unsigned int index;
	unsigned int parent;
	std::list<unsigned int> children;
};

class CUISystem : public ISystem
{
public:
	CUISystem(CGame *game = nullptr) { Initialize(game); }
	~CUISystem();

	bool Initialize(CGame *game);
	void Print(const std::shared_ptr<CFont> &font, int x, int y, const std::string &text);
	bool Update();

	bool CreateUIElement(unsigned int parent, const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties, bool hidden, unsigned int &handle);
	bool CreateUIElement(unsigned int parent, ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties, bool hidden, unsigned int &handle);
	void DestroyUIElement(unsigned int handle);
	UIElementData &GetUIElement(unsigned int handle);

	const std::list<unsigned int> &GetUIElementHandles() const { return m_uiElementHandles; }

	bool GetDisplayFPS() const { return m_displayFPS; }
	void SetDisplayFPS(bool displayFPS) { m_displayFPS = displayFPS; }
	int GetScale() const { return m_scale; }
	void SetScale(int scale) { m_scale = scale; m_buildUIElements = true; }

private:
	void BuildUIElements();

	CPool<UIElementData> m_uiElementPool;
	std::list<unsigned int> m_uiElementHandles;
	bool m_buildUIElements;
	bool m_displayFPS;
	int m_scale;
};

#endif