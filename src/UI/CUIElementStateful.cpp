#include "CUIElementStateful.h"

CUIElement &CUIElementStateful::AddState(const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties)
{
	m_states.push_back(CUIElement(m_game, m_parent, rect, texture, properties, true));

	return m_states.back();
}

CUIElement &CUIElementStateful::AddState(ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties)
{
	m_states.push_back(CUIElement(m_game, m_parent, xAlignment, yAlignment, texture, properties, true));

	return m_states.back();
}

void CUIElementStateful::SetState(unsigned int state)
{
	if (!m_parent->IsHidden())
	{
		m_states[m_state].SetHidden(true);
		m_states[state].SetHidden(false);
	}

	m_state = state;
}

bool CUIElementStateful::GetHidden() const
{
	return m_states[m_state].GetHidden();
}

void CUIElementStateful::SetHidden(bool hidden)
{
	m_states[m_state].SetHidden(hidden);
}