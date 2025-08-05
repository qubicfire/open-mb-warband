#include "skeleton.h"

using namespace brf;

bool Bone::load(FileStreamReader& stream)
{
    m_attach = stream.read<int>();
    m_name = stream.read_or_default("bone");

    if (m_name == "bone")
        int temp = stream.read<int>();

    m_x = stream.read<glm::vec3>();
    m_y = stream.read<glm::vec3>();
    m_z = stream.read<glm::vec3>();
    m_t = stream.read<glm::vec3>();

    if (m_attach >= 0)
        adjust_coordinate(this);
    else
        adjust_coordinate_half(this);

    return true;
}

constexpr float base_adjust_matrix[4][4] =
{
    0, 0, 1, 0,
    0, -1, 0, 0,
    1, 0, 0, 0,
    0, 0, 0, 1,
};

glm::vec3 Bone::adjust_coordinate(const glm::vec3& p)
{
    glm::vec3 s {};
    s.x = base_adjust_matrix[0][0] * p.x + base_adjust_matrix[0][1] * p.y + base_adjust_matrix[0][2] * p.z + base_adjust_matrix[0][3];
    s.y = base_adjust_matrix[1][0] * p.x + base_adjust_matrix[1][1] * p.y + base_adjust_matrix[1][2] * p.z + base_adjust_matrix[1][3];
    s.z = base_adjust_matrix[2][0] * p.x + base_adjust_matrix[2][1] * p.y + base_adjust_matrix[2][2] * p.z + base_adjust_matrix[2][3];
    float w = base_adjust_matrix[3][0] * p.x + base_adjust_matrix[3][1] * p.y + base_adjust_matrix[3][2] * p.z + base_adjust_matrix[3][3];

    if (w != 0)
        s /= w;

    return s;
}

glm::vec4 Bone::adjust_coordinate(const glm::vec4& point)
{
    return glm::vec4(point.x, point.w, -point.z, point.y);
}

glm::vec4 Bone::adjust_coordinate_half(const glm::vec4& point)
{
    static glm::vec4 q(0.0f, 1.0f / std::sqrtf(2.0f), 0.0f, 1.0f / std::sqrtf(2.0f));

    glm::vec3 t1(point.x, point.y, point.z);
    glm::vec3 t2(q.x, q.y, q.z);

    float d = glm::dot(t1, t2);
    glm::vec3 t3 = glm::cross(t1, t2);

    t1 *= q.x;
    t2 *= point.x;

    glm::vec3 tf = t1 + t2 + t3;

    return glm::vec4(point.x * q.x - d, tf.x, tf.y, tf.z);
}

void Bone::adjust_coordinate(Bone* bone)
{
    bone->m_x = adjust_coordinate(bone->m_z);
    bone->m_y = -adjust_coordinate(bone->m_y);
    bone->m_z = adjust_coordinate(bone->m_x);
    bone->m_t = adjust_coordinate(bone->m_t);
}

void Bone::adjust_coordinate_half(Bone* bone)
{
    const glm::vec3 temp = bone->m_x;

    bone->m_x = bone->m_z;
    bone->m_y = -bone->m_y;
    bone->m_z = temp;
}

bool Skeleton::load(FileStreamReader& stream)
{
    m_name = stream.read_with_length();
    uint32_t bones_count = stream.read<uint32_t>();
    m_bones.resize(bones_count);

    for (auto& bone : m_bones)
        bone.load(stream);

    m_root = -1;
    for (uint32_t i = 0; i < m_bones.size(); i++)
    {
        Bone& bone = m_bones[i];
        int attach = bone.m_attach;

        if (attach == -1)
            m_root = i;
        else
            bone.m_next.push_back(i);
    }

    return true;
}
