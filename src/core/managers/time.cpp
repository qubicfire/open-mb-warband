#include "core/platform/window.h"

#include "time.h"

using namespace mbcore;

uint32_t Time::get_frame() noexcept
{
    return m_frame;
}

float Time::get_time() noexcept
{
    return g_platform->get_time();
}

float Time::get_delta_time() noexcept
{
    return m_delta;
}

float Time::get_fps() noexcept
{
    return m_fps;
}

void Time::process_next_frame() noexcept
{
    const float current_time = g_platform->get_time();

    m_delta = current_time - m_last_time;
    m_last_time = current_time;
    m_frame++;

    m_fps = 1.0f / m_delta;
}
