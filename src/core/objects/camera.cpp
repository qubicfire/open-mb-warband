#include <glm/ext/matrix_clip_space.hpp>

#include "core/engine/engine.h"

#include "core/managers/input.h"
#include "core/managers/time.h"

#include "camera.h"

void Camera::start_client()
{
    m_last_offset_x = g_engine->get_width() / 2.0f;
    m_last_offset_y = g_engine->get_height() / 2.0f;
    m_sensitivity = 0.1f;
    m_fov = 60.0f;
    m_near = 0.1f;
    m_far = 1000.0f;
    m_yaw = -90.0f; // hack: fix first camera jump
    m_pitch = 0.0f;
    m_origin = glm::vec3(0.0f, 0.0f, 10.0f);
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_front = glm::vec3(0.0f, 0.0f, -1.0f);
    m_speed = 5.0f * Time::get_delta_time();

    set_object_flag(Object::Flags::Invisible);
    update_view_matrix();
}

void Camera::update()
{
    if (Input::get_key_down(KeyCode::Z))
    {
        m_disabled = !m_disabled;
        Input::set_cursor_visible(m_disabled);
    }

    if (Input::get_mouse_button_down(MouseCode::Left))
    {
        RayCastInfo info {};

        if (Physics::raycast(screen_point_to_ray(), info))
        {
            log_print("Origin: %.06f %.06f %.06f",
                info.m_hit_point.x,
                info.m_hit_point.y,
                info.m_hit_point.z);

            MapOriginPacket packet(info.m_hit_point);
            g_client->send(packet);
        }
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
        m_speed = 10.0f * Time::get_delta_time();
    else
        m_speed = 5.0f * Time::get_delta_time();

    if (Input::has_mouse_origin_changed())
    {
        const glm::vec2& mouse_origin = Input::get_mouse_origin();
        float offset_x = mouse_origin.x - m_last_offset_x;
        float offset_y = m_last_offset_y - mouse_origin.y;

        m_last_offset_x = mouse_origin.x;
        m_last_offset_y = mouse_origin.y;

        offset_x = offset_x * m_sensitivity;
        offset_y = offset_y * m_sensitivity;

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

        m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }

    m_view = glm::lookAt(m_origin,
        m_origin + m_front,
        m_up);
}

void Camera::update_view_matrix()
{
    m_projection = glm::perspective(glm::radians(m_fov),
        g_engine->get_aspect_ratio(),
        m_near,
        m_far);
}

Ray Camera::screen_point_to_ray()
{
    const glm::vec2& mouse_origin = Input::get_mouse_origin();

    float x = (2.0f * mouse_origin.x) / g_engine->get_width() - 1.0f;
    float y = 1.0f - (2.0f * mouse_origin.y) / g_engine->get_height();

    // ѕреобразование в координаты клип пространства
    glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::mat4 inverse_projection = glm::inverse(m_projection);
    glm::mat4 inverse_view = glm::inverse(m_view);

    // ѕреобразование в координаты глаза (eye space)
    glm::vec4 ray_eye = inverse_projection * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

    // ѕреобразование в мировые координаты
    glm::vec4 ray_world = inverse_view * ray_eye;
    glm::vec3 direction = glm::normalize(glm::vec3(ray_world));

    // ѕолучение позиции камеры (начала луча)
    glm::vec3 origin = glm::vec3(inverse_view[3]);

    // ѕолучение точки на дальней плоскости
    ray_clip = glm::vec4(x, y, 1.0f, 1.0f);
    ray_eye = inverse_projection * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, 1.0f, 1.0f);
    glm::vec4 world_origin = inverse_view * ray_eye;
    glm::vec3 end_origin = glm::vec3(world_origin) / world_origin.w;
    return Ray(end_origin, direction, 100.0f);
}

void Camera::set_fov(const float fov)
{
    m_fov = fov;
}

const float Camera::get_fov() const
{
    return m_fov;
}

const glm::vec3& Camera::get_front() const
{
    return m_front;
}

const glm::vec3& Camera::get_right() const
{
    return m_right;
}

const glm::vec3& Camera::get_up() const
{
    return m_up;
}

const glm::mat4& Camera::get_projection() const
{
    return m_projection;
}

const glm::mat4& Camera::get_inverse_projection() const
{
    return m_inverse_projection;
}

const glm::mat4& Camera::get_view() const
{
    return m_view;
}

Frustum Camera::create_frustum()
{
    Frustum frustum {};
    m_inverse_projection = m_projection * m_view;

    frustum.m_planes[Frustum::Left] = glm::vec4(
        m_inverse_projection[0][3] + m_inverse_projection[0][0],
        m_inverse_projection[1][3] + m_inverse_projection[1][0],
        m_inverse_projection[2][3] + m_inverse_projection[2][0],
        m_inverse_projection[3][3] + m_inverse_projection[3][0]
    );

    // Right plane
    frustum.m_planes[Frustum::Right] = glm::vec4(
        m_inverse_projection[0][3] - m_inverse_projection[0][0],
        m_inverse_projection[1][3] - m_inverse_projection[1][0],
        m_inverse_projection[2][3] - m_inverse_projection[2][0],
        m_inverse_projection[3][3] - m_inverse_projection[3][0]
    );

    // Bottom plane
    frustum.m_planes[Frustum::Bottom] = glm::vec4(
        m_inverse_projection[0][3] + m_inverse_projection[0][1],
        m_inverse_projection[1][3] + m_inverse_projection[1][1],
        m_inverse_projection[2][3] + m_inverse_projection[2][1],
        m_inverse_projection[3][3] + m_inverse_projection[3][1]
    );

    // Top plane
    frustum.m_planes[Frustum::Top] = glm::vec4(
        m_inverse_projection[0][3] - m_inverse_projection[0][1],
        m_inverse_projection[1][3] - m_inverse_projection[1][1],
        m_inverse_projection[2][3] - m_inverse_projection[2][1],
        m_inverse_projection[3][3] - m_inverse_projection[3][1]
    );

    // Near plane
    frustum.m_planes[Frustum::Near] = glm::vec4(
        m_inverse_projection[0][3] + m_inverse_projection[0][2],
        m_inverse_projection[1][3] + m_inverse_projection[1][2],
        m_inverse_projection[2][3] + m_inverse_projection[2][2],
        m_inverse_projection[3][3] + m_inverse_projection[3][2]
    );

    // Far plane
    frustum.m_planes[Frustum::Far] = glm::vec4(
        m_inverse_projection[0][3] - m_inverse_projection[0][2],
        m_inverse_projection[1][3] - m_inverse_projection[1][2],
        m_inverse_projection[2][3] - m_inverse_projection[2][2],
        m_inverse_projection[3][3] - m_inverse_projection[3][2]
    );

    for (int i = 0; i < Frustum::PlaneCount; i++) 
    {
        float length = glm::length(glm::vec3(frustum.m_planes[i]));
        frustum.m_planes[i] /= length;
    }

    return frustum;
}

bool Frustum::is_visible(const AABB& aabb) const
{
    glm::vec3 center = (aabb.m_min + aabb.m_max) * 0.5f;
    glm::vec3 extents = (aabb.m_max - aabb.m_min) * 0.5f;

    for (int i = 0; i < Frustum::PlaneCount; i++)
    {
        glm::vec3 normal = glm::vec3(m_planes[i]);
        float distance = glm::dot(normal, center) + m_planes[i].w;
        float radius = glm::dot(extents, glm::abs(normal));

        if (distance < -radius)
            return false;
    }

    return true;
}
