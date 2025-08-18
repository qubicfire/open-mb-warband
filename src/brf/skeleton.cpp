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

glm::mat4 Bone::get_rotation_matrix() const
{
    return glm::mat4(m_x[0], m_x[1], m_x[2], 0.0f,
        m_y[0], m_y[1], m_y[2], 0.0f,
        m_z[0], m_z[1], m_z[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

constexpr float base_adjust_matrix[4][4] =
{
    0, 0, 1, 0,
    0, -1, 0, 0,
    1, 0, 0, 0,
    0, 0, 0, 1,
};

static constexpr glm::mat4 base_matrix(
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, -1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

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

glm::quat Bone::adjust_coordinate(const glm::quat& point)
{
    return glm::quat(point.x, point.w, -point.z, point.y);
}

glm::mat4 brf::Bone::adjust_coordinate(const glm::mat4& matrix)
{
    return base_matrix * matrix * base_matrix;
}

glm::quat Bone::adjust_coordinate_half(const glm::quat& point)
{
    static glm::vec4 q(0.0f, 1.0f / std::sqrtf(2.0f), 0.0f, 1.0f / std::sqrtf(2.0f));

    glm::vec3 t1(point.x, point.y, point.z);
    glm::vec3 t2(q.x, q.y, q.z);

    float d = glm::dot(t1, t2);
    glm::vec3 t3 = glm::cross(t1, t2);

    t1 *= q.x;
    t2 *= point.x;

    glm::vec3 tf = t1 + t2 + t3;

    return glm::quat(point.x * q.x - d, tf.x, tf.y, tf.z);
}

glm::mat4 Bone::adjust_coordinate_half(const glm::mat4& matrix)
{
    return base_matrix * matrix;
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

std::vector<glm::mat4> Skeleton::get_bone_matrices(const AnimationFrame& frame)
{
    std::vector<glm::mat4> matrices {};
    glm::mat4 identity = glm::mat4(1.0f);
    const size_t bones_count = m_bones.size();

    matrices.resize(bones_count);

    set_bone_matrices(frame, matrices, identity, m_root);

    return matrices;
}

std::vector<glm::mat4> Skeleton::get_bone_matrices()
{
    std::vector<glm::mat4> matrices {};
    glm::mat4 identity = glm::mat4(1.0f);
    const size_t bones_count = m_bones.size();

    matrices.resize(bones_count);

    set_bone_matrices(matrices, identity, m_root);

    return matrices;
}

std::vector<glm::mat4> Skeleton::get_bone_matrices_inverse()
{
    std::vector<glm::mat4> matrices{};
    glm::mat4 identity = glm::mat4(1.0f);
    const size_t bones_count = m_bones.size();

    matrices.resize(bones_count);

    set_bone_matrices_inverse(matrices, identity, m_root);

    return matrices;
}

void Skeleton::set_bone_matrices(const AnimationFrame& frame,
    std::vector<glm::mat4>& output,
    const glm::mat4& identity, 
    int index)
{
    const Bone& bone = m_bones[index];
    glm::mat4 mat = glm::translate(glm::mat4(1.0f), bone.m_t);

    output[index] = identity * mat * glm::transpose(frame.get_rotation_matrix(index));

    for (const auto& next : bone.m_next)
        set_bone_matrices(frame, output, output[index], next);
}

void Skeleton::set_bone_matrices(std::vector<glm::mat4>& output,
    const glm::mat4& identity,
    int index)
{
    const Bone& bone = m_bones[index];
    glm::mat4 mat = glm::translate(glm::mat4(1.0f), bone.m_t);

    output[index] = identity * mat * glm::transpose(bone.get_rotation_matrix());

    for (const auto& next : bone.m_next)
        set_bone_matrices(output, output[index], next);
}

void Skeleton::set_bone_matrices_inverse(std::vector<glm::mat4>& output, 
    const glm::mat4& identity, 
    int index)
{
    const Bone& bone = m_bones[index];
    glm::mat4 mat = glm::translate(glm::mat4(1.0f), -bone.m_t);

    output[index] = bone.get_rotation_matrix() * mat * identity;

    for (const auto& next : bone.m_next)
        set_bone_matrices_inverse(output, output[index], next);
}
