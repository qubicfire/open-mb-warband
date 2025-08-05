#include "input.h"
#include "time.h"

void Input::initialize(const Unique<mbcore::Window>& window) noexcept
{
    m_window = window.get();
}

void Input::set_cursor_visible(bool state)
{
    m_window->set_cursor_visible(state);
}

bool Input::get_mouse_button(const MouseCode code) noexcept
{
    return m_mouse_keys[static_cast<int16_t>(code)];
}

bool Input::get_mouse_button_down(const MouseCode code) noexcept
{
    const int16_t key = static_cast<int16_t>(code);
    return m_mouse_keys[key]
        && m_mouse_frames[key] == Time::get_frame();
}

bool Input::get_mouse_button_up(const MouseCode code) noexcept
{
    const int16_t key = static_cast<int16_t>(code);
    return !m_mouse_keys[key]
        && m_mouse_frames[key] == Time::get_frame();
}

bool Input::has_mouse_origin_changed() noexcept
{
    return m_mouse_origin_frame == Time::get_frame();
}

glm::vec2& Input::get_mouse_origin() noexcept
{
    return m_mouse_origin;
}

bool Input::get_key(const KeyCode code) noexcept
{
    return m_keys[static_cast<int16_t>(code)];
}

bool Input::get_key_down(const KeyCode code) noexcept
{
    const int16_t key = static_cast<int16_t>(code);
    return m_keys[key]
        && m_frames[key] == Time::get_frame();
}

bool Input::get_key_up(const KeyCode code) noexcept
{
    const int16_t key = static_cast<int16_t>(code);
    return !m_keys[key]
        && m_frames[key] == Time::get_frame();
}

KeyCode Input::get_any_pressed_key() noexcept
{
    return m_last_key;
}

float Input::get_axis(const InputAxis axis)
{
    return 0.0f;
}

glm::vec3 Input::get_axis_dimension(const InputAxis axis)
{
    return glm::vec3();
}

void Input::set_mouse_button_pressed(const int code)
{
    bool& status = m_mouse_keys[code];

    if (!status)
    {
        status = true;
        m_mouse_frames[code] = Time::get_frame();
    }
}

void Input::set_mouse_button_released(const int code)
{
    m_mouse_keys[code] = false;
    m_mouse_frames[code] = Time::get_frame();
}

void Input::set_mouse_origin(const double x, const double y)
{
    m_mouse_origin.x = static_cast<float>(x);
    m_mouse_origin.y = static_cast<float>(y);
    m_mouse_origin_frame = Time::get_frame();
}

void Input::set_key_pressed(const int code)
{
    bool& status = m_keys[code];

    if (!status)
    {
        status = true;
        m_last_key = static_cast<KeyCode>(code);
        m_frames[code] = Time::get_frame();
    }
}

void Input::set_key_released(const int code)
{
    m_last_key = KeyCode::None;
    m_keys[code] = false;
    m_frames[code] = Time::get_frame();
}
