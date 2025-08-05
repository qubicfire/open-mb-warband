#ifndef _PROFILER_H
#define _PROFILER_H
#include <chrono>
#include <string>

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

	~Profiler() noexcept;

	void stop() noexcept;
private:
	std::chrono::time_point<std::chrono::steady_clock> m_start, m_end;
	std::chrono::duration<float> m_duration;
	std::string m_name;
	bool m_is_stopped;
};

#define profiler_start() Profiler profiler(__FUNCTION__);\

#endif // !_PROFILER_H
