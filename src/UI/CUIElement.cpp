#include "CUIElement.h"
#include "Game/CGame.h"

CUIElement::CUIElement(CGame *game, const CUIElement *parent, const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties, bool hidden) : m_game(game)
{
	m_game->GetUISystem().CreateUIElement(parent->m_handle, rect, texture, properties, hidden, m_handle);

	m_initialized = true;
}

CUIElement::CUIElement(CGame *game, const CUIElement *parent, ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties, bool hidden) : m_game(game)
{
	m_game->GetUISystem().CreateUIElement(parent->m_handle, xAlignment, yAlignment, texture, properties, hidden, m_handle);

	m_initialized = true;
}

CUIElement::~CUIElement()
{
	if (m_game != nullptr)
		m_game->GetUISystem().DestroyUIElement(m_handle);
}

bool CUIElement::Initialize(CGame *game, const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties, bool hidden)
{
	if (game != nullptr)
	{
		m_game = game;

		m_game->GetUISystem().CreateUIElement(UINT_MAX, rect, texture, properties, hidden, m_handle);

		m_initialized = true;
	}

	return m_initialized;
}

bool CUIElement::Initialize(CGame *game, ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties, bool hidden)
{
	if (game != nullptr)
	{
		m_game = game;

		m_game->GetUISystem().CreateUIElement(UINT_MAX, xAlignment, yAlignment, texture, properties, hidden, m_handle);

		m_initialized = true;
	}

	return m_initialized;
}

void CUIElement::Print(const std::shared_ptr<CFont> &font, int x, int y, const std::string &text)
{
	UIElementData &uiElement = m_game->GetUISystem().GetUIElement(m_handle);

	int scale = m_game->GetUISystem().GetScale();

	x = uiElement.absoluteRect.x + x * scale;
	y = uiElement.absoluteRect.y + y * scale;

	m_game->GetUISystem().Print(font, x, y, text);
}

CUIElement &CUIElement::AddChild(const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties, bool hidden)
{
	m_children.push_back(CUIElement(m_game, this, rect, texture, properties, hidden));

	return m_children.back();
}

CUIElement &CUIElement::AddChild(ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties, bool hidden)
{
	m_children.push_back(CUIElement(m_game, this, xAlignment, yAlignment, texture, properties, hidden));

	return m_children.back();
}

CUIElementStateful &CUIElement::AddStatefulChild()
{
	m_statefulChildren.push_back(CUIElementStateful(m_game, this));

	return m_statefulChildren.back();
}

bool CUIElement::GetHidden() const
{
	UIElementData &uiElement = m_game->GetUISystem().GetUIElement(m_handle);
	return uiElement.hidden;
}

void CUIElement::SetHidden(bool hidden)
{
	UIElementData &uiElement = m_game->GetUISystem().GetUIElement(m_handle);
	uiElement.hidden = hidden;

	for (auto it = m_children.begin(); it != m_children.end(); ++it)
		(*it).SetHidden(hidden);

	for (auto it = m_statefulChildren.begin(); it != m_statefulChildren.end(); ++it)
		(*it).SetHidden(hidden);
}
