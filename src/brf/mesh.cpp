#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"

using namespace brf;
using namespace mbcore;

static inline int MESH_VERSION;

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
        MESH_VERSION = 1;
    else 
        MESH_VERSION = 2;

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

    if (skinning.size() > 0)
    {
        m_skinning.resize(origins_count);

        m_max_bone_index = std::numeric_limits<int>::min();
        for (const auto& skin : skinning)
        {
            for (const auto& rigging_pair : skin.m_pairs)
            {
                m_skinning[rigging_pair.m_vertex_index].add(
                    skin.m_bone_index,
                    rigging_pair.m_weight
                );

                if (m_max_bone_index < skin.m_bone_index)
                    m_max_bone_index = skin.m_bone_index;
            }
        }
    }

    for (uint32_t i = 0; i < m_indices.size(); i += 3)
    {
        const int a = stream.read<int>();
        const int b = stream.read<int>();
        const int c = stream.read<int>();

        Vertex& v_a = m_vertices[a];
        Vertex& v_b = m_vertices[b];
        Vertex& v_c = m_vertices[c];

        v_a.m_origin = frame->m_origins[v_a.m_index];
        v_b.m_origin = frame->m_origins[v_b.m_index];
        v_c.m_origin = frame->m_origins[v_c.m_index];

        // Setup bone's indexes and weights
        if (m_skinning.size() > 0)
        {
            v_a.m_bone_index = m_skinning[v_a.m_index].cast_bone_index();
            v_a.m_bone_weight = m_skinning[v_a.m_index].cast_bone_weight();

            v_b.m_bone_index = m_skinning[v_b.m_index].cast_bone_index();
            v_b.m_bone_weight = m_skinning[v_b.m_index].cast_bone_weight();

            v_c.m_bone_index = m_skinning[v_c.m_index].cast_bone_index();
            v_c.m_bone_weight = m_skinning[v_c.m_index].cast_bone_weight();
        }

        m_indices[i] = a;
        m_indices[i + 1] = b;
        m_indices[i + 2] = c;
    }

    frame->m_normals.resize(vertexes_count);

    mbutils::transform(m_vertices,
        frame->m_normals,
        [](const brf::Vertex& a, glm::vec3& b) -> void {
            b = a.m_normal;
        }
    );

    return true;
}

void Mesh::precache(int flags)
{
    if (m_vertex_array)
        return; 

    m_vertex_array = VertexArray::create();
    Unique<VertexBuffer> vertex_buffer = VertexBuffer::create(m_vertices, flags | BufferFlags::Persistent);

    // TODO: IMPLEMENT KEYFRAME ANIMATION SYSTEM
    // https://www.mbsoftworks.sk/tutorials/opengl3/24-animation-pt1-keyframe-md2/
    m_vertex_array->link(0, VertexType::Float3, cast_offset(brf::Vertex, m_origin));
    m_vertex_array->link(1, VertexType::Float3, cast_offset(brf::Vertex, m_normal));
    m_vertex_array->link(2, VertexType::Float2, cast_offset(brf::Vertex, m_texture_a));
    m_vertex_array->link(3, VertexType::Uint, cast_offset(brf::Vertex, m_color));
    m_vertex_array->link(4, VertexType::Float4, cast_offset(brf::Vertex, m_bone_index));
    m_vertex_array->link(5, VertexType::Float4, cast_offset(brf::Vertex, m_bone_weight));

    Unique<IndexBuffer> index_buffer = IndexBuffer::create(m_indices);

    m_vertex_array->set_vertex_buffer(vertex_buffer);
    m_vertex_array->set_index_buffer(index_buffer);

    m_vertex_array->unbind();
}

int Mesh::get_max_bone() const
{
    return m_max_bone_index;
}

const std::string& Mesh::get_name() const
{
    return m_name;
}

const std::string& Mesh::get_material() const
{
    return m_material;
}

const std::vector<Frame>& Mesh::get_frames() const
{
    return m_frames;
}

const std::vector<uint32_t>& Mesh::get_indices() const
{
    return m_indices;
}

const std::vector<Vertex>& Mesh::get_vertices() const
{
    return m_vertices;
}

void Mesh::apply_for_batching(std::vector<Vertex>& batch_vertices,
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

    switch (MESH_VERSION)
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

glm::vec4 brf::Skinning::cast_bone_index() const
{
    return glm::vec4(m_bone_index[0], m_bone_index[1], m_bone_index[2], m_bone_index[3]);
}

glm::vec4 brf::Skinning::cast_bone_weight() const
{
    return glm::vec4(m_bone_weight[0], m_bone_weight[1], m_bone_weight[2], m_bone_weight[3]);
}
