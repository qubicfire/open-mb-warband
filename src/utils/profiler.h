#ifndef _PROFILER_H
#define _PROFILER_H
#include <chrono>
#include <string>

#include "utils/log.h"

class Profiler
{
public:
	explicit Profiler(const std::string& name) noexcept
		: m_name(name)
		, m_duration(0)
		, m_is_stopped(false)
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	explicit Profiler(std::string&& name) noexcept
		: m_name(std::move(name))
		, m_duration(0)
		, m_is_stopped(false)
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	~Profiler()
	{
		stop();
	}

	void stop()
	{
		if (m_is_stopped)
			return;

		m_end = std::chrono::high_resolution_clock::now();
		m_duration = m_end - m_start;

		float ms = m_duration.count() * 1000.0f;
		log_success("Profiler \'%s\' - %.6f ms", m_name.c_str(), ms);

		m_is_stopped = true;
	}
private:
	std::chrono::time_point<std::chrono::steady_clock> m_start, m_end;
	std::chrono::duration<float> m_duration;
	std::string m_name;
	bool m_is_stopped;
};

#ifdef _DEBUG

	#define profiler_start(name)			\
		Profiler name(__FUNCTION__);		\

	#define profiler_fast_start()			\
		profiler_start(_FAST_PROFILER_)		\

	#define profiler_stop(name)				\
		name.stop();						\

	#define profiler_fast_stop()			\
		profiler_stop(_FAST_PROFILER_)		\

#else

	#define profiler_start(name) void(0)
	#define profiler_fast_start() void(0)
	#define profiler_stop() void(0)
	#define profiler_fast_stop() void(0)

#endif // _DEBUG

#endif // !_PROFILER_H
