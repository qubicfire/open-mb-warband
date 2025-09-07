#include "core/managers/assets.h"
#include "utils/thread_pool.h"

#include "resource.h"

using namespace brf;

template <class _Tx>
inline static void load_resource_part(FileStreamReader& stream,
    mb_small_array<_Tx>& array)
{
    uint32_t length = stream.read<uint32_t>();
    array.resize(length);

    for (auto& element : array)
        element.load(stream);
}

static inline std::mutex s_mutex {};

bool Resource::load(const std::string& path)
{
    FileStreamReader stream;
    if (!stream.open(path))
        return false;

    uint32_t version {};
    while (true)
    {
        const std::string_view token = stream.read_with_length_fast();

        if (token == "end")
            break;
        else if (token == "rfver ")
            version = stream.read<uint32_t>();
        else if (token == "mesh")
            load_resource_part<Mesh>(stream, m_meshes);
        else if (token == "texture")
            load_resource_part<Texture>(stream, m_textures);
        else if (token == "material")
            load_resource_part<Material>(stream, m_materials);
        else if (token == "skeleton")
            load_resource_part<Skeleton>(stream, m_skeletons);
        else if (token == "skeleton_anim")
            load_resource_part<Animation>(stream, m_animations);
        else if (token == "body")
            load_resource_part<Body>(stream, m_bodies);
    }

    {
        std::lock_guard<std::mutex> lock(s_mutex);

        for (auto& mesh : m_meshes)
            g_assets->add_mesh(&mesh);
    }

    return true;
}

Mesh* Resource::get_mesh(const std::string& name)
{
    for (auto& mesh : m_meshes)
    {
        if (mesh.get_name() == name)
            return &mesh;
    }

    return nullptr;
}

mb_small_array<Mesh>& Resource::get_meshes()
{
    return m_meshes;
}

const uint32_t Resource::get_meshes_count() const
{
    return static_cast<uint32_t>(
        m_meshes.size()
    );
}
