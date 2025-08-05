#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"

using namespace brf;

namespace mbutils
{
    template <class _Ty, 
        class _Kx, 
        class _Func,
        class _ForEachType = uint32_t>
    void transform(const std::vector<_Ty>& source, 
        std::vector<_Kx>& destination,
        _Func function)
    {
        for (_ForEachType index = 0; index < destination.size(); index++)
            function(source[index], destination[index]);
    }
}

struct TmpRiggingPair
{
    int m_vertex_index;
    float m_weight;
};

struct TmpSkinning 
{
    bool load(FileStreamReader& stream);

    int m_bone_index; // bone index
    std::vector<TmpRiggingPair> m_pairs;
};

bool Mesh::load(FileStreamReader& stream)
{
    constexpr int RESERVE_FIRST_FRAME = 1;
    m_frames.resize(RESERVE_FIRST_FRAME);

    m_name = stream.read_with_length();
    uint32_t flags = stream.read<uint32_t>();
    m_material = stream.read_with_length();

    if (flags & (1 << 16))
        s_version = 1;
    else 
        s_version = 2;

    uint32_t origins_count = stream.read<uint32_t>();
    Frame* frame = &m_frames.back();
    frame->m_origins.resize(origins_count);

    for (auto& origin : frame->m_origins)
        origin = stream.read<glm::vec3>();

    std::vector<TmpSkinning> skinning {};
    uint32_t rigs_count = stream.read<uint32_t>();
    if (rigs_count > 0)
    {
        skinning.resize(rigs_count);

        for (auto& rig : skinning)
            rig.load(stream);
    }

    uint32_t frames_count = stream.read<uint32_t>();
    if (frames_count > 0)
    {
        m_frames.resize(frames_count + 1);
        frame = &m_frames.front();

        for (uint32_t i = 1; i < m_frames.size(); i++)
            m_frames[i].load(stream);
    }

    uint32_t vertexes_count = stream.read<uint32_t>();
    m_vertices.resize(vertexes_count);

    for (auto& vertex : m_vertices)
        vertex.load(stream);

    uint32_t faces_count = stream.read<uint32_t>();
    m_indices.resize(faces_count * 3);

    for (uint32_t i = 0; i < m_indices.size(); i += 3)
    {
        int a = stream.read<int>();
        int b = stream.read<int>();
        int c = stream.read<int>();

        m_vertices[a].m_origin = frame->m_origins[m_vertices[a].m_index];
        m_vertices[b].m_origin = frame->m_origins[m_vertices[b].m_index];
        m_vertices[c].m_origin = frame->m_origins[m_vertices[c].m_index];

        m_indices[i] = a;
        m_indices[i + 1] = b;
        m_indices[i + 2] = c;

       /* m_indices[i] = c;
        m_indices[i + 1] = b;
        m_indices[i + 2] = a;*/
    }

    frame->m_normals.resize(vertexes_count);

    mbutils::transform(m_vertices,
        frame->m_normals,
        [](const brf::Vertex& a, glm::vec3& b) -> void {
            b = a.m_normal;
        }
    );

    if (skinning.size() > 0)
    {
        m_skinning.resize(origins_count);

        int max = std::numeric_limits<int>::min();
        for (uint32_t i = 0; i < skinning.size(); i++)
        {
            const auto& temp_rig = skinning[i];
            for (uint32_t j = 0; j < temp_rig.m_pairs.size(); j++)
            {
                const auto& rigging_pair = temp_rig.m_pairs[j];

                m_skinning[rigging_pair.m_vertex_index].add(
                    temp_rig.m_bone_index,
                    rigging_pair.m_weight
                );

                if (max < temp_rig.m_bone_index)
                    max = temp_rig.m_bone_index;
            }
        }
    }

    return true;
}

const std::string& brf::Mesh::get_name() const noexcept
{
    return m_name;
}

const std::string& brf::Mesh::get_material() const noexcept
{
    return m_material;
}

const std::vector<Frame>& brf::Mesh::get_frames() const noexcept
{
    return m_frames;
}

const std::vector<uint32_t>& brf::Mesh::get_indices() const noexcept
{
    return m_indices;
}

const std::vector<Vertex>& brf::Mesh::get_vertices() const noexcept
{
    return m_vertices;
}

void brf::Mesh::apply_for_batching(std::vector<Vertex>& batch_vertices,
    std::vector<uint32_t>& batch_indices,
    const glm::vec3& origin, 
    const glm::vec3& rotation, 
    const glm::vec3& scale)
{
    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), origin) *
        glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(-rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::scale(glm::mat4(1.0f), scale);

    for (const auto& vertex : m_vertices)
    {
        Vertex translated_vertex {};
        translated_vertex.m_color = vertex.m_color;
        translated_vertex.m_index = vertex.m_index;
        translated_vertex.m_normal = vertex.m_normal;
        translated_vertex.m_origin = glm::vec3(model * glm::vec4(vertex.m_origin, 1.0f));
        translated_vertex.m_tangent = vertex.m_tangent;
        translated_vertex.m_tbn = vertex.m_tbn;
        translated_vertex.m_texture_a = vertex.m_texture_a;
        translated_vertex.m_texture_b = vertex.m_texture_b;
        batch_vertices.push_back(std::move(translated_vertex));
    }

    const uint32_t total_indices = static_cast<uint32_t>(
        batch_indices.size()
    );

    for (const auto& indice : m_indices)
        batch_indices.push_back(total_indices + indice);
}

#ifdef _DEBUG
const std::pair<const char*, float> brf::Mesh::get_memory_size() const
{
    float memory = m_frames.size() * sizeof(Frame)
        + m_vertices.size() * sizeof(Vertex)
        + m_skinning.size() * sizeof(Skinning)
        + m_indices.size() * sizeof(uint32_t);

    for (const auto& frame : m_frames)
        memory = memory + frame.m_normals.size() * sizeof(glm::vec3) + frame.m_origins.size() * sizeof(glm::vec3);

    if (memory >= 1024.0f * 1024.0f)
        return std::make_pair("Memory (MB): %.03f", memory / (1024.0f * 1024.0f));
    else if (memory >= 1024.0f)
        return std::make_pair("Memory (KB): %.03f", memory / 1024.0f);
    else
        return std::make_pair("Memory (BYTES): %.03f", memory);
}
#endif // _DEBUG

bool Frame::load(FileStreamReader& stream)
{
    m_time = stream.read<int>();

    uint32_t origins_count = stream.read<uint32_t>();
    m_origins.resize(origins_count);

    for (auto& origin : m_origins)
        origin = stream.read<glm::vec3>();

    uint32_t normals_count = stream.read<uint32_t>();
    m_normals.resize(normals_count);

    for (auto& normal : m_normals)
        normal = stream.read<glm::vec3>();

    return true;
}

const glm::vec3 Frame::min() const
{
    return glm::vec3();
}

const glm::vec3 Frame::max() const
{
    return glm::vec3();
}

int Frame::find_closest_point(const glm::vec3& point, const float max_distance) const
{
    return 0;
}

bool Vertex::load(FileStreamReader& stream)
{
    m_index = stream.read<int>();
    m_color = stream.read<uint32_t>();
    m_normal = stream.read<glm::vec3>();

    switch (Mesh::s_version)
    {
        case 0:
            m_texture_a = stream.read<glm::vec2>();
            m_texture_b = stream.read<glm::vec2>();

            m_texture_a.y = 1 - m_texture_a.y;
            m_texture_b.y = 1 - m_texture_b.y;
            break;
        case 1:
            m_tangent = stream.read<glm::vec3>();
            m_tbn = stream.read<uint8_t>();

            m_texture_a = stream.read<glm::vec2>();
            m_texture_a.y = 1 - m_texture_a.y;
            m_texture_b = m_texture_a;
            break;
        case 2:
            m_texture_a = stream.read<glm::vec2>();
            m_texture_a.y = 1 - m_texture_a.y;
            m_texture_b = m_texture_a;
            break;
    }

    return true;
}

bool Face::load(FileStreamReader& stream)
{
    i = stream.read<int>();
    j = stream.read<int>();
    k = stream.read<int>();
    return true;
}

bool TmpSkinning::load(FileStreamReader& stream)
{
    m_bone_index = stream.read<int>();
    uint32_t pairs_count = stream.read<uint32_t>();
    m_pairs.resize(pairs_count);

    for (auto& pair : m_pairs)
    {
        pair.m_vertex_index = stream.read<int>();
        pair.m_weight = stream.read<float>();
    }

    return true;
}

int Skinning::try_get_empty() const
{
    for (int i = 0; i < 4; i++)
    {
        if (m_bone_index[i] == -1)
            return i;
    }

    return 4;
}

int Skinning::get_smallest_index_width() const
{
    int min = 0;

    for (int k = 1; k < 4; k++)
    {
        if (m_bone_index[k] != -1 && m_bone_weight[k] < m_bone_weight[min])
            min = k;
    }

    return min;
}

void Skinning::normalize()
{
    float sum = 0;

    for (int k = 0; k < 4; k++)
    {
        if (m_bone_index[k] != -1) 
            sum = sum + m_bone_weight[k];
    }

    if (sum == 0) 
        return;

    for (int k = 0; k < 4; k++)
    {
        if (m_bone_index[k] != -1) 
            m_bone_weight[k] /= sum;
    }
}

void Skinning::add(const int index, const float weight)
{
    bool overflow = false;
    int bone_index = try_get_empty();

    if (bone_index >= 4) 
    {
        bone_index = get_smallest_index_width();
        overflow = true;
    }

    m_bone_index[bone_index] = index;
    m_bone_weight[bone_index] = weight;

    if (overflow) 
        normalize();
}