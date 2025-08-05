// TODO: Remove this later
#include "core/platform/opengl/opengl.h"

#include "time.h"

uint32_t Time::get_frame() noexcept
{
    return m_frame;
}

float Time::get_time() noexcept
{
    // TODO: implement a normal solution, bro
    return glfwGetTime();
}

float Time::get_delta_time() noexcept
{
    return m_delta;
}

void Time::process_next_frame() noexcept
{
    const float current_time = static_cast<float>(
        glfwGetTime()
    );

    m_delta = current_time - m_last_time;
    m_last_time = current_time;
    m_frame++;
}
