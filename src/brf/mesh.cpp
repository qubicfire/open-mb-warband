#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/managers/assets.h"

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
    void transform(const mb_small_array<_Ty>& source,
        mb_small_array<_Kx>& destination,
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

Mesh::Mesh(mb_unique<mbcore::VertexArray>& array) noexcept
    : m_vertex_array(std::move(array))
{
}

bool Mesh::load(FileStreamReader& stream, std::string& name)
{
    constexpr int RESERVE_FIRST_FRAME = 1;
    m_frames.resize(RESERVE_FIRST_FRAME);

    name = stream.read_with_length();
    uint32_t flags = stream.read<uint32_t>();
    m_material = stream.read_with_length();

    if (flags & (1 << 16))
        MESH_VERSION = 1;
    else 
        MESH_VERSION = 2;

    uint32_t origins_count = stream.read<uint32_t>();
    Frame* frame = &m_frames.back();
    frame->m_origins.resize(origins_count);
    frame->m_min = glm::vec3(0.0f);
    frame->m_max = glm::vec3(0.0f);

    for (auto& origin : frame->m_origins)
    {
        origin = stream.read<glm::vec3>();

        frame->m_min.x = glm::min(origin.x, frame->m_min.x);
        frame->m_min.y = glm::min(origin.y, frame->m_min.y);
        frame->m_min.z = glm::min(origin.z, frame->m_min.z);

        frame->m_max.x = glm::max(origin.x, frame->m_max.x);
        frame->m_max.y = glm::max(origin.y, frame->m_max.y);
        frame->m_max.z = glm::max(origin.z, frame->m_max.z);
    }

    mb_small_array<TmpSkinning> skinning {};
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
    m_faces.resize(faces_count);

    if (skinning.size() > 0)
    {
        m_skinning.resize(origins_count);

        m_bone = std::numeric_limits<int>::min();
        for (const auto& skin : skinning)
        {
            for (const auto& rigging_pair : skin.m_pairs)
            {
                m_skinning[rigging_pair.m_vertex_index].add(
                    skin.m_bone_index,
                    rigging_pair.m_weight
                );

                if (m_bone < skin.m_bone_index)
                    m_bone = skin.m_bone_index;
            }
        }
    }

    for (auto& face : m_faces)
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

        face = Face(a, b, c);
    }

    frame->m_normals.resize(vertexes_count);

    mbutils::transform(m_vertices,
        frame->m_normals,
        [](const brf::Vertex& a, glm::vec3& b) -> void {
            b = a.m_normal;
        }
    );

    for (auto& frame : m_frames)
        frame.resort(m_vertices);

    return true;
}

void Mesh::precache(AABB& aabb, const Buffer::Types flags)
{
    if (m_frames.empty())
        return;

    const Frame& frame = m_frames.front();
    aabb = AABB{ frame.m_min, frame.m_max };

    // mesh already initialized, so no need to do it twice
    if (m_vertex_array)
        return;

    m_vertex_array = VertexArray::create(VertexArray::Indexes);
    mb_unique<Buffer> vertex_buffer = Buffer::create(m_vertices, Buffer::Types::Array | flags);

    m_vertex_array->link(0, VertexType::Float3, cast_offset(brf::Vertex, m_origin));
    m_vertex_array->link(1, VertexType::Float3, cast_offset(brf::Vertex, m_normal));
    m_vertex_array->link(2, VertexType::Float2, cast_offset(brf::Vertex, m_texture_a));
    m_vertex_array->link(3, VertexType::Uint, cast_offset(brf::Vertex, m_color));
    m_vertex_array->link(4, VertexType::Float4, cast_offset(brf::Vertex, m_bone_index));
    m_vertex_array->link(5, VertexType::Float4, cast_offset(brf::Vertex, m_bone_weight));
    m_vertex_array->link(6, VertexType::Float3, cast_offset(brf::Vertex, m_origin));
    m_vertex_array->link(7, VertexType::Float3, cast_offset(brf::Vertex, m_normal));

    mb_unique<Buffer> index_buffer = Buffer::create(m_faces, Buffer::Types::Element);

    m_vertex_array->set_vertex_buffer(vertex_buffer);
    m_vertex_array->set_index_buffer(index_buffer);

    const size_t origins_size = frame.m_origins.size() * sizeof(glm::vec3);
    const size_t normals_size = frame.m_normals.size() * sizeof(glm::vec3);

    m_frame_buffers.resize(m_frames.size());
    for (int index = 0; index < m_frames.size(); index++)
    {
        mb_unique<Buffer> frame_vertex_buffer = Buffer::create(Buffer::Types::Array);

        frame_vertex_buffer->buffer_data(origins_size + normals_size, nullptr, Buffer::Types::Static);
        frame_vertex_buffer->sub_data(0, origins_size, m_frames[index].m_origins.m_array);
        frame_vertex_buffer->sub_data(origins_size, normals_size, m_frames[index].m_normals.m_array);

        m_frame_buffers[index] = std::move(frame_vertex_buffer);
    }

    m_vertex_array->unbind();
}

void Mesh::update_frame_vertices()
{
    m_vertex_array->bind();

    const mb_unique<Buffer>& buffer = m_frame_buffers[m_frame];

    buffer->bind();

    const size_t normals_offset = m_frames
        .front()
        .m_origins
        .size() * sizeof(glm::vec3);

    m_vertex_array->link(6, VertexType::Float3, sizeof(glm::vec3), nullptr);
    m_vertex_array->link(7, VertexType::Float3, sizeof(glm::vec3), (void*)normals_offset);

    m_vertex_array->unbind();
}

void Mesh::set_frame(const int frame)
{
    m_frame = frame;
}

int Mesh::get_bone() const
{
    return m_bone;
}

int Mesh::get_frame() const
{
    return m_frame;
}

const std::string& Mesh::get_material() const
{
    return m_material;
}

const std::vector<Frame>& Mesh::get_frames() const
{
    return m_frames;
}

const mb_small_array<Face>& Mesh::get_faces() const
{
    return m_faces;
}

const mb_small_array<Vertex>& Mesh::get_vertices() const
{
    return m_vertices;
}

const mb_unique<VertexArray>& Mesh::get_vertex_array() const
{
    return m_vertex_array;
}

bool Frame::load(FileStreamReader& stream)
{
    m_min = glm::vec3(0.0f);
    m_max = glm::vec3(0.0f);
    m_time = static_cast<float>(
        stream.read<int>()
    );

    uint32_t origins_count = stream.read<uint32_t>();
    m_origins.resize(origins_count);

    for (auto& origin : m_origins)
    {
        origin = stream.read<glm::vec3>();

        m_min.x = glm::min(origin.x, m_min.x);
        m_min.y = glm::min(origin.y, m_min.y);
        m_min.z = glm::min(origin.z, m_min.z);

        m_max.x = glm::max(origin.x, m_max.x);
        m_max.y = glm::max(origin.y, m_max.y);
        m_max.z = glm::max(origin.z, m_max.z);
    }

    uint32_t normals_count = stream.read<uint32_t>();
    m_normals.resize(normals_count);

    for (auto& normal : m_normals)
        normal = stream.read<glm::vec3>();

    return true;
}

void Frame::resort(const mb_small_array<Vertex>& vertices)
{
    const size_t new_size = vertices.size();

    mb_small_array<glm::vec3> origins(m_origins);
    mb_small_array<glm::vec3> normals(m_normals);

    m_origins.resize(new_size);
    
    // normals always have the same count as vertices 
    // so only reallocate origins
#if 0
    if (new_size > m_normals.size())
        m_normals.resize(new_size);
#endif
    
    int index = 0;
    for (const auto& vertex : vertices)
    {
        const int vertex_index = vertex.m_index;

        if (vertex_index >= 0 && vertex_index < m_origins.size()) 
        {
            m_origins[index] = origins[vertex_index];
            m_normals[index] = normals[vertex_index];
        }
        else 
        {
            m_origins[index] = vertex.m_origin;
            m_normals[index] = vertex.m_normal;
        }

        index++;
    }
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
    return glm::vec4(
        m_bone_index[0],
        m_bone_index[1],
        m_bone_index[2], 
        m_bone_index[3]
    );
}

glm::vec4 brf::Skinning::cast_bone_weight() const
{
    return glm::vec4(
        m_bone_weight[0],
        m_bone_weight[1],
        m_bone_weight[2],
        m_bone_weight[3]
    );
}

void MeshBuilder::mesh_apply(Mesh* mesh)
{
    g_assets->add_mesh(mesh);
}
