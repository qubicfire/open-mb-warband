#include "core/net/client_interface.h"
#include "core/net/server_interface.h"

#include "object.h"

void Object::set_origin(const glm::vec3& origin) noexcept
{
    m_origin = origin;
}

void Object::set_rotation(const glm::vec3& rotation) noexcept
{
    m_rotation = rotation;
}

void Object::set_scale(const glm::vec3& scale) noexcept
{
    m_scale = scale;
}

void Object::set_angle(const float angle) noexcept
{
    m_angle = angle;
}

const glm::vec3& Object::get_origin() const noexcept
{
    return m_origin;
}

const glm::vec3& Object::get_rotation() const noexcept
{
    return m_rotation;
}

const glm::vec3& Object::get_scale() const noexcept
{
    return m_scale;
}

const float Object::get_angle() const noexcept
{
    return m_angle;
}

const uint32_t Object::get_id() const noexcept
{
    return m_id;
}

void Object::start_internal()
{
    if (g_server_interface)
        start();

    if (g_client_interface)
        client_start();
}

void Object::server_build_object_info(Packet& packet)
{

}
