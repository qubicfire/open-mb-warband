#include "core/net/client.h"
#include "core/net/server.h"

#include "object.h"

#include "core/managers/objects.h"

using namespace mbcore;

void Object::add_mesh(brf::Mesh* mesh)
{
    m_meshes.push_back(mesh);
}

void Object::add_texture(Texture2D* texture)
{
    m_textures.push_back(texture);
}

void Object::add_texture(const std::string& path, const Texture2D::Type type)
{
    Texture2D* texture = Texture2D::create(path, type);
    add_texture(texture);
}

void Object::add_child(Object* child)
{
    m_childs.push_back(child);
}

void Object::set_object_flag(const ObjectFlags flags)
{
    m_flags.set(const_cast<ObjectFlags>(flags));
}

void Object::set_object_flags(const ObjectFlags flags)
{
    m_flags.set_bits(flags);
}

void Object::set_aabb(const glm::vec3& min, const glm::vec3& max)
{
    m_aabb = AABB{ min, max };
}

void Object::set_parent(Object* parent)
{
    parent->add_child(this);
}

void Object::set_origin(const glm::vec3& origin)
{
    m_origin = origin;

    m_flags.set(ObjectFlags::DirtyMatrix);
}

void Object::set_rotation(const glm::vec3& rotation)
{
    m_rotation = rotation;

    m_flags.set(ObjectFlags::DirtyMatrix);
}

void Object::set_scale(const glm::vec3& scale)
{
    m_scale = scale;

    m_flags.set(ObjectFlags::DirtyMatrix);
}

void Object::set_angle(const float angle)
{
    m_angle = angle;

    m_flags.set(ObjectFlags::DirtyMatrix);
}

const glm::mat4& Object::get_transform()
{
    if (m_flags.try_clear_bit(ObjectFlags::DirtyMatrix))
    {
        m_transform = glm::translate(m_transform, m_origin);
        m_transform = glm::rotate(m_transform, glm::radians(m_angle), m_rotation);
        m_transform = glm::scale(m_transform, m_scale);

        return m_transform;
    }

    return m_transform;
}

const mb_bit_set<Object::ObjectFlags> Object::get_object_flags() const
{
    return m_flags;
}

const AABB& Object::get_aabb() const
{
    return AABB{ m_aabb.m_min + m_origin, m_aabb.m_max + m_origin };
}

const glm::vec3& Object::get_origin() const
{
    return m_origin;
}

const glm::vec3& Object::get_rotation() const
{
    return m_rotation;
}

const glm::vec3& Object::get_scale() const
{
    return m_scale;
}

const glm::vec3& Object::get_front() const
{
    return -m_transform[2];
}

const glm::vec3& Object::get_right() const
{
    return m_transform[0];
}

const glm::vec3& Object::get_up() const
{
    return m_transform[1];
}

const float Object::get_angle() const
{
    return m_angle;
}

const uint32_t Object::get_id() const
{
    return m_id;
}

brf::Mesh* Object::get_mesh() const
{
    return *m_meshes.begin();
}

void Object::start_internal()
{
    start();

    if (Client::is_running())
        start_client();
}

void Object::instantiate_internal(Object* object)
{
    g_objects->add_object(object);
}

std::list<brf::Mesh*>& Object::get_meshes()
{
    return m_meshes;
}

Texture2D* Object::get_texture() const
{
    return *m_textures.begin();
}

std::list<Texture2D*>& Object::get_textures()
{
    return m_textures;
}
