#include <cmath>

#include "CTimer.h"
#include "Interface/IPlatform.h"

bool CTimer::Initialize(IPlatform *platform)
{
	if (platform != nullptr)
	{
		m_platform = platform;

		m_initialized = true;
	}

	return m_initialized;
}

void CTimer::Update(unsigned int lockedFrameRate)
{
	if (!m_initialized)
		return;

	if (m_lastTime == 0)
		m_lastTime = m_platform->GetTicks();

	m_currentTime = m_platform->GetTicks();
	double elapsedTime = (m_currentTime - m_lastTime) * 0.001;

	if (lockedFrameRate != 0)
	{
		while (elapsedTime < 1.0 / lockedFrameRate)
		{
			m_currentTime = m_platform->GetTicks();
			elapsedTime = (m_currentTime - m_lastTime) * 0.001;
		}
	}

	m_lastTime = m_currentTime;

	if (fabs(elapsedTime - m_elapsedTime) < 1.0)
	{
		m_timeSamples[m_timeSampleIndex] = elapsedTime;
		m_timeSampleIndex = (m_timeSampleIndex + 1) & 63;

		if (m_timeSampleCount < 64)
			m_timeSampleCount++;
	}

	m_frameCount++;
	m_timeElapsed += m_elapsedTime;

	if (m_timeElapsed > 1.0)
	{
		m_frameRate = m_frameCount;
		m_frameCount = 0;
		m_timeElapsed = 0.0;
		m_frameRateText = std::to_string(m_frameRate) + " FPS";

		if (int(m_frameRate) < m_minimumFrameRate)
			m_minimumFrameRate = m_frameRate;

		if (int(m_frameRate) > m_maximumFrameRate)
			m_maximumFrameRate = m_frameRate;

		m_averageFrameRateCount++;

		if (m_averageFrameRateCount == 0)
		{
			m_averageFrameRate = 0.0f;
			m_averageFrameRateCount = 1;
		}

		m_averageFrameRate += (m_frameRate - m_averageFrameRate) / m_averageFrameRateCount;
	}

	m_elapsedTime = 0.0;

	for (unsigned int i = 0; i < m_timeSampleCount; i++)
		m_elapsedTime += m_timeSamples[i];

	if (m_timeSampleCount > 0)
		m_elapsedTime /= m_timeSampleCount;
}