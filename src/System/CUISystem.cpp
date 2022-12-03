#include <vector>

#include "CUISystem.h"
#include "Game/CGame.h"
#include "Resource/CFont.h"

CUISystem::~CUISystem()
{
	if (m_initialized)
		m_game->GetGameLoop().UnregisterSystem(*this);
}

bool CUISystem::Initialize(CGame *game)
{
	if (ISystem::Initialize(game))
	{
		m_game->GetGameLoop().RegisterSystem(*this);

		m_game->GetConsole().RegisterVariable("ui_display_fps", false, [this](bool &value) { SetDisplayFPS(value); });
		m_game->GetConsole().RegisterVariable("ui_scale", 1, [this](int &value) { value = glm::clamp(value, 1, 4); SetScale(value); });

		m_initialized = true;
	}

	return m_initialized;
}

void CUISystem::Print(const std::shared_ptr<CFont> &font, int x, int y, const std::string &text)
{
	m_game->GetRenderSystem().Print(font, x, y, text);
}

bool CUISystem::Update()
{
	BuildUIElements();

	if (m_displayFPS)
		Print(m_game->GetFont(), 1 * m_scale, m_game->GetRenderer()->GetViewportHeight() - (m_game->GetFont()->GetCellHeight() + 3) * m_scale, m_game->GetTimer().GetFrameRateText());

	return false;
}

bool CUISystem::CreateUIElement(unsigned int parent, const UIElementRect &rect, const UIElementTexture &texture, ECUIElementProperties properties, bool hidden, unsigned int &handle)
{
	UIElementData uiElement;
	uiElement.absoluteRect = { 0, 0, 0, 0 };
	uiElement.relativeRect = rect;
	uiElement.xAlignment = ECUIElementHorizontalAlignment::None;
	uiElement.yAlignment = ECUIElementVerticalAlignment::None;
	uiElement.texture = m_game->GetResourceManager().AcquireResource<CTexture>(texture.name);
	uiElement.texture->SetFilter(ECTextureFilter::Point);
	uiElement.textureRect = texture.rect;
	uiElement.properties = properties;
	uiElement.hidden = hidden;
	uiElement.index = m_uiElementHandles.size();
	uiElement.parent = parent;

	handle = m_uiElementPool.Add(uiElement);
	m_uiElementHandles.push_back(handle);

	if (parent != UINT_MAX)
		m_uiElementPool.Get(parent).children.push_back(handle);

	m_buildUIElements = true;

	return true;
}

bool CUISystem::CreateUIElement(unsigned int parent, ECUIElementHorizontalAlignment xAlignment, ECUIElementVerticalAlignment yAlignment, const UIElementTexture &texture, ECUIElementProperties properties, bool hidden, unsigned int &handle)
{
	UIElementData uiElement;
	uiElement.absoluteRect = { 0, 0, 0, 0 };
	uiElement.relativeRect = { 0, 0, 0, 0 };
	uiElement.xAlignment = xAlignment;
	uiElement.yAlignment = yAlignment;
	uiElement.texture = m_game->GetResourceManager().AcquireResource<CTexture>(texture.name);
	uiElement.texture->SetFilter(ECTextureFilter::Point);
	uiElement.textureRect = texture.rect;
	uiElement.properties = properties;
	uiElement.hidden = hidden;
	uiElement.index = m_uiElementHandles.size();
	uiElement.parent = parent;

	handle = m_uiElementPool.Add(uiElement);
	m_uiElementHandles.push_back(handle);

	if (parent != UINT_MAX)
		m_uiElementPool.Get(parent).children.push_back(handle);

	m_buildUIElements = true;

	return true;
}

void CUISystem::DestroyUIElement(unsigned int handle)
{
	unsigned int parent = m_uiElementPool.Get(handle).parent;

	if (parent != UINT_MAX)
		m_uiElementPool.Get(parent).children.remove(handle);

	m_uiElementHandles.remove(handle);
	m_uiElementPool.Remove(handle);

	m_buildUIElements = true;
}

UIElementData &CUISystem::GetUIElement(unsigned int handle)
{
	return m_uiElementPool.Get(handle);
}

void CUISystem::BuildUIElements()
{
	if (!m_buildUIElements)
		return;

	int viewportWidth = m_game->GetRenderer()->GetViewportWidth();
	int viewportHeight = m_game->GetRenderer()->GetViewportHeight();

	unsigned int index = 0;

	for (auto it = m_uiElementHandles.begin(); it != m_uiElementHandles.end(); ++it)
	{
		unsigned int handle = *it;
		UIElementData &uiElement = m_uiElementPool.Get(handle);

		if (uiElement.xAlignment != ECUIElementHorizontalAlignment::None)
		{
			if ((uiElement.properties & ECUIElementProperties::FillParentWidth) == ECUIElementProperties::FillParentWidth)
			{
				if (uiElement.parent != UINT_MAX)
					uiElement.absoluteRect.w = m_uiElementPool.Get(uiElement.parent).absoluteRect.w;
				else
					uiElement.absoluteRect.w = viewportWidth;
			}
			else
				uiElement.absoluteRect.w = uiElement.textureRect.w * m_scale;

			if (uiElement.parent != UINT_MAX)
			{
				UIElementData &parentElement = m_uiElementPool.Get(uiElement.parent);

				if (uiElement.xAlignment == ECUIElementHorizontalAlignment::Left)
					uiElement.absoluteRect.x = parentElement.absoluteRect.x;
				else if (uiElement.xAlignment == ECUIElementHorizontalAlignment::Right)
					uiElement.absoluteRect.x = (parentElement.absoluteRect.x + parentElement.absoluteRect.w) - uiElement.absoluteRect.w;
				else if (uiElement.xAlignment == ECUIElementHorizontalAlignment::Center)
					uiElement.absoluteRect.x = ((parentElement.absoluteRect.x + parentElement.absoluteRect.w) - uiElement.absoluteRect.w) / 2;
			}
			else
			{
				if (uiElement.xAlignment == ECUIElementHorizontalAlignment::Left)
					uiElement.absoluteRect.x = 0;
				else if (uiElement.xAlignment == ECUIElementHorizontalAlignment::Right)
					uiElement.absoluteRect.x = viewportWidth - uiElement.absoluteRect.w;
				else if (uiElement.xAlignment == ECUIElementHorizontalAlignment::Center)
					uiElement.absoluteRect.x = (viewportWidth - uiElement.absoluteRect.w) / 2;
			}
		}
		else
		{
			uiElement.absoluteRect.x = uiElement.relativeRect.x * m_scale;

			if (uiElement.parent != UINT_MAX)
				uiElement.absoluteRect.x += m_uiElementPool.Get(uiElement.parent).absoluteRect.x;

			uiElement.absoluteRect.w = uiElement.relativeRect.w * m_scale;
		}

		if (uiElement.yAlignment != ECUIElementVerticalAlignment::None)
		{
			if ((uiElement.properties & ECUIElementProperties::FillParentHeight) == ECUIElementProperties::FillParentHeight)
			{
				if (uiElement.parent != UINT_MAX)
					uiElement.absoluteRect.h = m_uiElementPool.Get(uiElement.parent).absoluteRect.h;
				else
					uiElement.absoluteRect.h = viewportHeight;
			}
			else
				uiElement.absoluteRect.h = uiElement.textureRect.h * m_scale;

			if (uiElement.parent != UINT_MAX)
			{
				UIElementData &parentElement = m_uiElementPool.Get(uiElement.parent);

				if (uiElement.yAlignment == ECUIElementVerticalAlignment::Top)
					uiElement.absoluteRect.y = parentElement.absoluteRect.y;
				else if (uiElement.yAlignment == ECUIElementVerticalAlignment::Bottom)
					uiElement.absoluteRect.y = (parentElement.absoluteRect.y + parentElement.absoluteRect.h) - uiElement.absoluteRect.h;
				else if (uiElement.yAlignment == ECUIElementVerticalAlignment::Center)
					uiElement.absoluteRect.y = ((parentElement.absoluteRect.y + parentElement.absoluteRect.h) - uiElement.absoluteRect.h) / 2;
			}
			else
			{
				if (uiElement.yAlignment == ECUIElementVerticalAlignment::Top)
					uiElement.absoluteRect.y = 0;
				else if (uiElement.yAlignment == ECUIElementVerticalAlignment::Bottom)
					uiElement.absoluteRect.y = viewportHeight - uiElement.absoluteRect.h;
				else if (uiElement.yAlignment == ECUIElementVerticalAlignment::Center)
					uiElement.absoluteRect.y = (viewportHeight - uiElement.absoluteRect.h) / 2;
			}
		}
		else
		{
			uiElement.absoluteRect.y = uiElement.relativeRect.y * m_scale;

			if (uiElement.parent != UINT_MAX)
				uiElement.absoluteRect.y += m_uiElementPool.Get(uiElement.parent).absoluteRect.y;

			uiElement.absoluteRect.h = uiElement.relativeRect.h * m_scale;
		}

		uiElement.index = index++;
	}

	m_game->GetRenderSystem().UpdateUI(true);

	m_buildUIElements = false;
}