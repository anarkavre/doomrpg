#ifndef __CTIMER_H__
#define __CTIMER_H__

#include <string>

class IPlatform;

class CTimer
{
public:
	CTimer(IPlatform *platform = nullptr) { Initialize(platform); }

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
	bool m_initialized{false};
	IPlatform *m_platform{nullptr};
	unsigned int m_lastTime{0}, m_currentTime{0};
	double m_elapsedTime{0.0};
	double m_timeSamples[64]{0.0};
	unsigned int m_timeSampleIndex{0}, m_timeSampleCount{0};
	unsigned int m_frameCount{0}, m_frameRate{0};
	float m_averageFrameRate{0.0f};
	unsigned int m_averageFrameRateCount{0};
	int m_minimumFrameRate{INT_MAX}, m_maximumFrameRate{INT_MIN};
	double m_timeElapsed{0.0};
	std::string m_frameRateText;
};

#endif