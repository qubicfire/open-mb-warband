#include "body.h"

using namespace brf;

bool Body::load(FileStreamReader& stream)
{
    m_name = stream.read_with_length();
    std::string_view settings = stream.read_with_length_fast();

    if (settings == "composite")
    {
        uint32_t parts_count = stream.read<uint32_t>();
        m_parts.resize(parts_count);

        for (auto& part : m_parts)
            part.load(stream, settings);
    }
    else
    {
        BodyPart part {};

        part.load(stream, settings);

        m_parts.push_back(part);
    }

    calculate_bbox();

    return true;
}

void Body::calculate_bbox()
{
    m_bbox.set_default();

    for (const auto& part : m_parts)
    {
        Box3f bbox = part.calculate_bbox();

        m_bbox.add(bbox);
    }
}

bool BodyPart::load(FileStreamReader& stream, std::string_view settings)
{
    std::string_view body_part_settings {};

    if (settings.size() <= 0)
        body_part_settings = stream.read_with_length_fast();
    else
        body_part_settings = settings;

    if (body_part_settings == "manifold")
    {
        m_type = BodyPartType::Manifold;

        uint32_t points_count = stream.read<uint32_t>();
        m_points.resize(points_count);

        for (auto& point : m_points)
            point = stream.read<glm::vec3>();

        int faces_count = stream.read<int>();
        for (int i = 0; i < faces_count; i++)
        {
            m_orientation = stream.read<int>();

            std::vector<int> verts {};
            int flags = stream.read<int>(); // not used?
            int verts_count = stream.read<int>();

            verts.resize(verts_count);

            for (auto& vert : verts)
                vert = stream.read<int>();

            m_faces.push_back(std::move(verts));
        }
    }
    else if (body_part_settings == "capsule")
    {
        m_type = BodyPartType::Capsule;
        m_radius = stream.read<float>();

        m_center = stream.read<glm::vec3>();
        m_direction = stream.read<glm::vec3>();

        m_flags = stream.read<int>();
    }
    else if (body_part_settings == "sphere")
    {
        m_type = BodyPartType::Sphere;
        m_radius = stream.read<float>();

        m_center = stream.read<glm::vec3>();

        m_flags = stream.read<int>();
    }
    else if (body_part_settings == "face")
    {
        std::vector<int> faces {};

        m_type = BodyPartType::Face;

        uint32_t points_count = stream.read<uint32_t>();
        m_points.resize(points_count);

        for (auto& point : m_points)
            point = stream.read<glm::vec3>();

        faces.resize(points_count);

        for (uint32_t i = 0; i < points_count; i++)
            faces[i] = i;

        m_faces.push_back(std::move(faces));

        m_flags = stream.read<int>();
    }

    return true;
}
 
Box3f BodyPart::calculate_bbox() const
{
    Box3f bbox {};

    if (m_type == BodyPartType::Manifold || m_type == BodyPartType::Face)
    {
        for (const auto& point : m_points)
            bbox.add(point);
    }
    else if (m_type == BodyPartType::Sphere)
    {
        glm::vec3 radius(m_radius, m_radius, m_radius);

        bbox.add(m_center + radius);
        bbox.add(m_center - radius);
    }
    else if (m_type == BodyPartType::Capsule)
    {
        glm::vec3 radius(m_radius, m_radius, m_radius);

        bbox.add(m_center + radius);
        bbox.add(m_center - radius);
        bbox.add(m_direction + radius);
        bbox.add(m_direction - radius);
    }

    return std::move(bbox);
}
