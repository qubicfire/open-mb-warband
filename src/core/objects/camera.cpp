#include <glm/ext/matrix_clip_space.hpp>

#include "core/engine/engine.h"

#include "core/managers/input.h"
#include "core/managers/time.h"

#include "camera.h"

void Camera::start()
{
    m_last_offset_x = g_engine->get_width() / 2.0f;
    m_last_offset_y = g_engine->get_height() / 2.0f;
    m_sensitivity = 5.0f;
    m_fov = 60.0f;
    m_near = 0.1f;
    m_far = 1000.0f;
    m_yaw = -90.0f; // hack: fix first camera jump
    m_pitch = 0.0f;
    m_origin = glm::vec3(0.0f, 0.0f, 10.0f);
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_front = glm::vec3(0.0f, 0.0f, -1.0f);
    m_speed = 0.05f;

    update_view_matrix();
}

void Camera::update()
{
    if (Input::get_key_down(KeyCode::Z))
    {
        m_disabled = !m_disabled;
        Input::set_cursor_visible(m_disabled);
    }

    if (m_disabled)
        return;

    if (Input::get_key(KeyCode::W))
        m_origin = m_origin + m_speed * m_front;
    if (Input::get_key(KeyCode::S))
        m_origin = m_origin - m_speed * m_front;
    if (Input::get_key(KeyCode::A))
        m_origin = m_origin - m_speed * glm::normalize(glm::cross(m_front, m_up));
    if (Input::get_key(KeyCode::D))
        m_origin = m_origin + m_speed * glm::normalize(glm::cross(m_front, m_up));

    if (Input::get_key(KeyCode::LeftShift))
        m_origin = m_origin - m_speed * m_up;
    if (Input::get_key(KeyCode::Space))
        m_origin = m_origin + m_speed * m_up;
    if (Input::get_key(KeyCode::LeftControl))
        m_speed = 0.2f;
    else
        m_speed = 0.05f;

    if (Input::has_mouse_origin_changed())
    {
        const glm::vec2& mouse_origin = Input::get_mouse_origin();
        float offset_x = mouse_origin.x - m_last_offset_x;
        float offset_y = m_last_offset_y - mouse_origin.y;

        m_last_offset_x = mouse_origin.x;
        m_last_offset_y = mouse_origin.y;

        offset_x = offset_x * m_sensitivity * Time::get_delta_time();
        offset_y = offset_y * m_sensitivity * Time::get_delta_time();

        m_yaw = m_yaw + offset_x;
        m_pitch = m_pitch + offset_y;
        m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

        glm::vec3 direction = 
        {
            std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch)),
            std::sin(glm::radians(m_pitch)),
            std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch))
        };

        m_front = glm::normalize(direction);
    }
}

void Camera::update_view_matrix()
{
    m_projection = glm::perspective(glm::radians(m_fov),
        g_engine->get_aspect_ratio(),
        m_near,
        m_far);
}

void Camera::set_fov(const float fov)
{
    m_fov = fov;
}

const float Camera::get_fov() const noexcept
{
    return m_fov;
}

const glm::vec3& Camera::get_front() const noexcept
{
    return m_front;
}

const glm::vec3& Camera::get_up() const noexcept
{
    return m_up;
}

const glm::mat4& Camera::get_projection() const noexcept
{
    return m_projection;
}
