#include "CDialog.h"
#include "CDoomRPG.h"
#include "Camera/CTurnBasedCamera.h"
#include "Resource/CFont.h"

CDialog::~CDialog()
{
	if (m_doomrpg != nullptr)
		m_doomrpg->GetGameLoop().UnregisterUpdateHandler(*this);
}

bool CDialog::Initialize(CDoomRPG *doomrpg)
{
	if (doomrpg != nullptr)
	{
		m_doomrpg = doomrpg;

		m_dialog.Initialize(m_doomrpg, ECUIElementHorizontalAlignment::Center, ECUIElementVerticalAlignment::Bottom, { "dialog.png", 0, 0, 130, 55 });
		m_dialog.AddChild({ 123, 2, 5, 6 }, { "q.bmp", 1, 1, 5, 6 });
		m_dialog.AddChild({ 123, 47, 5, 6 }, { "q.bmp", 1, 7, 5, 6 });
		m_dialog.SetHidden(m_hidden);

		m_actions[0] = m_doomrpg->GetInputSystem().GetAction("action");
		m_actions[1] = m_doomrpg->GetInputSystem().GetAction("turnLeft");
		m_actions[2] = m_doomrpg->GetInputSystem().GetAction("turnRight");

		m_printTics = static_cast<unsigned int>(1.0f / 32.0f * m_doomrpg->GetGameLoop().GetUpdateRate());

		m_initialized = true;
	}

	return m_initialized;
}

bool CDialog::Update(float elapsedTime)
{
	if (!m_initialized || m_hidden)
		return false;

	if (m_printing)
	{
		m_printTicCount--;

		if (m_printTicCount == 0)
		{
			m_index++;

			if (m_index == m_lines[m_end - 1].length() - 1)
			{
				m_end++;
				m_index = 0;

				if (m_end > m_lines.size())
				{
					m_end = m_lines.size();
					m_index = m_lines[m_end - 1].length();
					m_printing = false;
				}
				else if (m_end - m_start - 1 == 4)
				{
					m_end--;
					m_index = m_lines[m_end - 1].length();
					m_printing = false;
				}
			}

			m_printTicCount = m_printTics;
		}
	}

	if (!m_locked && (m_actions[1]->IsPressed() || m_actions[2]->IsPressed()))
	{
		m_doomrpg->GetStatus().Show();

		static_cast<CTurnBasedCamera &>(m_doomrpg->GetCameraSystem().GetViewCamera()).Unlock();

		m_dialog.SetHidden(true);

		m_actions[1]->SetPressed(false);
		m_actions[2]->SetPressed(false);

		m_hidden = true;
		m_canceled = true;

		return true;
	}

	if (m_actions[0]->IsPressed())
	{
		if (m_printing)
		{
			m_end = glm::min(static_cast<unsigned int>(m_lines.size()), m_start + 4);
			m_index = m_lines[m_end - 1].length();
			m_printTicCount = m_printTics;
			m_printing = false;
		}
		else
		{
			if (m_end == m_lines.size())
			{
				m_doomrpg->GetStatus().Show();

				static_cast<CTurnBasedCamera &>(m_doomrpg->GetCameraSystem().GetViewCamera()).Unlock();

				m_dialog.SetHidden(true);

				m_hidden = true;
				m_locked = false;

				return true;
			}

			m_start += 4;
			m_end = m_start + 1;
			m_index = 0;
			m_printing = true;
		}

		m_actions[0]->SetPressed(false);
	}

	return false;
}

void CDialog::Render()
{
	if (!m_initialized || m_hidden)
		return;

	std::shared_ptr<CFont> font = m_doomrpg->GetFont();

	int x = 1;
	int y = 3;

	for (unsigned int i = m_start; i < m_end; i++)
	{
		if (i == m_end - 1)
			m_dialog.Print(font, x, y, m_lines[i].substr(0, m_index + 1));
		else
			m_dialog.Print(font, x, y, m_lines[i]);

		y += font->GetCellHeight();
	}
}

void CDialog::Show()
{
	if (!m_initialized || !m_hidden)
		return;

	m_doomrpg->GetStatus().Hide();

	static_cast<CTurnBasedCamera &>(m_doomrpg->GetCameraSystem().GetViewCamera()).Lock();

	m_dialog.SetHidden(false);

	m_doomrpg->GetGameLoop().RegisterUpdateHandler(*this);

	m_start = 0;
	m_end = 1;
	m_index = 0;
	m_printTicCount = m_printTics;
	m_printing = true;

	m_hidden = false;
	m_canceled = false;
}

void CDialog::Hide()
{
	if (!m_initialized || m_hidden)
		return;

	m_doomrpg->GetStatus().Show();

	static_cast<CTurnBasedCamera &>(m_doomrpg->GetCameraSystem().GetViewCamera()).Unlock();

	m_dialog.SetHidden(true);

	m_hidden = true;
}

void CDialog::ParseLines()
{
	m_lines.clear();

	size_t start = 0;

	while (start < m_message.length())
	{
		size_t end = m_message.find_first_of('|', start);

		if (end == std::string::npos)
			end = m_message.length();
		
		m_lines.push_back(m_message.substr(start, end - start));

		start = end + 1;
	}
}