#ifndef __CTIMER_H__
#define __CTIMER_H__

#include <string>

class IPlatform;

class CTimer
{
public:
	CTimer(IPlatform *platform = nullptr) : m_lastTime(0), m_elapsedTime(0.0), m_timeSampleIndex(0), m_timeSampleCount(0), m_frameCount(0), m_frameRate(0), m_averageFrameRate(0.0f), m_averageFrameRateCount(0), m_minimumFrameRate(INT_MAX), m_maximumFrameRate(INT_MIN), m_timeElapsed(0.0) { Initialize(platform); }

	bool Initialize(IPlatform *platform);
	void Update(unsigned int lockedFrameRate = 0);

	float GetElapsedTime() const { return static_cast<float>(m_elapsedTime); }
	unsigned int GetFrameRate() const { return m_frameRate; }
	float GetAverageFrameRate() const { return m_averageFrameRate; }
	unsigned int GetMinimumFrameRate() const { return m_minimumFrameRate; }
	unsigned int GetMaximumFrameRate() const { return m_maximumFrameRate; }
	const std::string &GetFrameRateText() const { return m_frameRateText; }

	bool IsInitialized() const { return m_initialized; }

private:
	bool m_initialized;
	IPlatform *m_platform;
	unsigned int m_lastTime, m_currentTime;
	double m_elapsedTime;
	double m_timeSamples[64];
	unsigned int m_timeSampleIndex, m_timeSampleCount;
	unsigned int m_frameCount, m_frameRate;
	float m_averageFrameRate;
	unsigned int m_averageFrameRateCount;
	int m_minimumFrameRate, m_maximumFrameRate;
	double m_timeElapsed;
	std::string m_frameRateText;
};

#endif