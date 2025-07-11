#pragma once
#include <chrono>
using namespace std::chrono;

namespace VORTEK_UTIL
{
class Timer
{
  private:
	time_point<steady_clock> m_StartPoint;
	time_point<steady_clock> m_PausedPoint;
	bool m_bIsRunning{ false };
	bool m_bIsPaused{ false };

  public:
	Timer() = default;
	~Timer() = default;

	void Start();
	void Stop();
	void Pause();
	void Resume();

	const int64_t ElapsedMS();
	const int64_t ElapsedSec();

	inline const bool IsRunning() const { return m_bIsRunning; }
	inline const bool IsPaused() const { return m_bIsPaused; }
};
} // namespace VORTEK_UTIL
