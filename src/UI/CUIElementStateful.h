#ifndef __CUIELEMENTSTATEFUL_H__
#define __CUIELEMENTSTATEFUL_H__

#include <list>
#include <memory>
#include <vector>

#include "UIElement.h"

class CGame;
class CUIElement;

class CUIElementStateful
{
public:
	CUIElementStateful(CGame *game, const CUIElement *parent) : m_game(game), m_parent(parent), m_state(0) {}
	~CUIElementStateful() {}

	CUIElement &AddState(const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties = ECUIElementProperties::None);
	CUIElement &AddState(ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties = ECUIElementProperties::None);

	unsigned int GetState() const { return m_state; }
	void SetState(unsigned int state);
	bool GetHidden() const;
	void SetHidden(bool hidden);

	bool IsHidden() const { return GetHidden(); }

private:
	CGame *m_game;
	const CUIElement *m_parent;
	std::vector<CUIElement> m_states;
	unsigned int m_state;
};

#endif