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
		, m_is_stopped(false)
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	explicit Profiler(std::string&& name) noexcept
		: m_name(std::move(name))
		, m_is_stopped(false)
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	~Profiler()
	{
		stop();
	}

	void stop(bool print_log = true)
	{
		if (m_is_stopped)
			return;

		m_end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = m_end - m_start;

		m_time_passed = duration.count() * 1000.0f;

		if (print_log)
			log_success("Profiler \'%s\' - %.6f ms", m_name.c_str(), m_time_passed);

		m_is_stopped = true;
	}

	float get_time() const
	{
		return m_time_passed;
	}
private:
	std::chrono::time_point<std::chrono::steady_clock> m_start, m_end;
	float m_time_passed;
	std::string m_name;
	bool m_is_stopped;
};

#ifdef _DEBUG

	#define profiler_start(name)			\
		Profiler name(__FUNCTION__);		\

	#define profiler_fast_start()			\
		profiler_start(_FAST_PROFILER_)		\

	#define profiler_stop(name, ...)		\
		name.stop(__VA_ARGS__);				\

	#define profiler_fast_stop()			\
		profiler_stop(_FAST_PROFILER_)		\

#else

	#define profiler_start(name) void(0)
	#define profiler_fast_start() void(0)
	#define profiler_stop() void(0)
	#define profiler_fast_stop() void(0)

#endif // _DEBUG

#endif // !_PROFILER_H
