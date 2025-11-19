#include "core/managers/assets.h"
#include "utils/thread_pool.h"

#include "resource.h"

using namespace brf;

template <class _Tx>
static inline void load_resource_part(FileStreamReader& stream,
    mb_small_array<_Tx>& array)
{
    uint32_t length = stream.read<uint32_t>();
    array.resize(length);

    for (auto& element : array)
    {
        if constexpr (std::is_same_v<_Tx, AssetsContoller::MeshTempSettings>)
        {
            Mesh* mesh = new Mesh();
            std::string name {};
            mesh->load(stream, name);

            element = AssetsContoller::MeshTempSettings{ mesh, name };
        }
        else
        {
            element.load(stream);
        }
    }
}

bool Resource::load(const std::string& path)
{
    FileStreamReader stream;
    if (!stream.open(path))
        return false;

    uint32_t version {};
    mb_small_array<AssetsContoller::MeshTempSettings> meshes {};

    while (true)
    {
        const std::string_view token = stream.read_with_length_fast();

        if (token == "end")
            break;
        else if (token == "rfver ")
            version = stream.read<uint32_t>();
        else if (token == "mesh")
            load_resource_part<AssetsContoller::MeshTempSettings>(stream, meshes);
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

    g_assets->add_meshes_with_unique_id(meshes);

    return true;
}
