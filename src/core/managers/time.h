#ifndef _TIME_H
#define _TIME_H
#include "core/mb.h"

class Time final
{
public:
	static uint32_t get_frame() noexcept;
	static float get_time() noexcept;
	static float get_delta_time() noexcept;
private:
	static void process_next_frame() noexcept;
private:
	static inline uint32_t m_frame {};
	static inline float m_delta {};
	static inline float m_last_time {};
};

#endif // !_TIME_H
