#include "profiler.h"

#include "utils/log.h"

Profiler::~Profiler() noexcept
{
	stop();
}

void Profiler::stop() noexcept
{
	if (!m_is_stopped)
	{
		m_end = std::chrono::high_resolution_clock::now();
		m_duration = m_end - m_start;

		float ms = m_duration.count() * 1000.0f;
		log_success("Profiler \'%s\' - %.6f ms", m_name.c_str(), ms);

		m_is_stopped = true;
	}
}
