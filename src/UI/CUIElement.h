#ifndef __CUIELEMENT_H__
#define __CUIELEMENT_H__

#include <list>
#include <memory>
#include <vector>

#include "UIElement.h"

class CFont;
class CGame;
class CUIElementStateful;

class CUIElement
{
public:
	CUIElement() {}
	CUIElement(const CUIElement &uiElement) = default;
	CUIElement(CUIElement &&uiElement) noexcept : m_initialized(std::exchange(uiElement.m_initialized, false)), m_game(std::exchange(uiElement.m_game, nullptr)), m_handle(std::exchange(uiElement.m_handle, UINT_MAX)), m_children(std::move(uiElement.m_children)) {}
	CUIElement(CGame *game, const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties = ECUIElementProperties::None, bool hidden = false) { Initialize(game, rect, texture, properties, hidden); }
	CUIElement(CGame *game, ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties = ECUIElementProperties::None, bool hidden = false) { Initialize(game, xAlignment, yAlignment, texture, properties, hidden); }
	CUIElement(CGame *game, const CUIElement *parent, const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties = ECUIElementProperties::None, bool hidden = false);
	CUIElement(CGame *game, const CUIElement *parent, ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties = ECUIElementProperties::None, bool hidden = false);
	~CUIElement();

	bool Initialize(CGame *game, const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties = ECUIElementProperties::None, bool hidden = false);
	bool Initialize(CGame *game, ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties = ECUIElementProperties::None, bool hidden = false);
	void Print(const std::shared_ptr<CFont> &font, int x, int y, const std::string &text);
	CUIElement &AddChild(const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties = ECUIElementProperties::None, bool hidden = false);
	CUIElement &AddChild(ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties = ECUIElementProperties::None, bool hidden = false);
	CUIElementStateful &AddStatefulChild();

	bool GetHidden() const;
	void SetHidden(bool hidden);

	bool IsInitialized() const { return m_initialized; }
	bool IsHidden() const { return GetHidden(); }

private:
	bool m_initialized{false};
	CGame *m_game{nullptr};
	unsigned int m_handle{0};
	std::list<CUIElement> m_children;
	std::list<CUIElementStateful> m_statefulChildren;
};

#endif