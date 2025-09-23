#include "core/managers/objects.h"

#include "assets.h"

using namespace mbcore;

Shader* AssetsContoller::load_shader(std::string_view key, 
    std::string_view vertex, 
    std::string_view fragment)
{
    mb_unique<Shader> shader_unique = Shader::create(vertex, fragment);
    Shader* shader = shader_unique.get();

    {
        std::lock_guard lock(m_mutex);
        m_shaders.emplace(key, std::move(shader_unique));
    }

    return shader;
}

Shader* AssetsContoller::load_shader(std::string_view key,
    std::string_view path, 
    Shader::ShaderType type)
{
    mb_unique<Shader> shader_unique = Shader::create(path, type);
    Shader* shader = shader_unique.get();

    {
        std::lock_guard lock(m_mutex);
        m_shaders.emplace(key, std::move(shader_unique));
    }

    return shader;
}

void AssetsContoller::remove_shader(std::string_view key)
{
    const auto& it = m_shaders.find(key);

    if (it != m_shaders.end())
        m_shaders.erase(it);
    else
        log_warning("Shader \'%s\' doesn't found?", key.data());
}

Shader* AssetsContoller::get_shader(std::string_view key) const
{
    const auto& it = m_shaders.find(key);
    return it != m_shaders.end() ? it->second.get() : nullptr;
}

brf::Resource* AssetsContoller::load_resource(const std::string& path)
{
    mb_unique<brf::Resource> resource_unique = create_unique<brf::Resource>();

    if (!resource_unique->load(path))
    {
        log_alert("Failed to load \'%s\' resource", path.c_str());
        return nullptr;
    }
    else
    {
        log_print(
            "Resource file \'%s\' loaded\n\t[%s]:\n",
            path.c_str(),
            path.c_str()
        );

        brf::Resource* resource = resource_unique.get();

        // Where's std version of concurrent vector or something like that?
        {
            std::lock_guard lock(m_mutex);
            m_resources.push_back(std::move(resource_unique));
        }

        return resource;
    }
}

Texture2D* AssetsContoller::load_texture(const std::string& path,
    const Texture2D::Type type)
{
    const auto& it = m_textures.find(path);

    if (it != m_textures.end())
        return it->second.get();

    mb_unique<Texture2D> unique_texture = Texture2D::create_internal(path, type);
    Texture2D* texture = unique_texture.get();

    m_textures.emplace(path, std::move(unique_texture));

    return texture;
}

void AssetsContoller::add_mesh(brf::Mesh* mesh)
{
    m_meshes.push_back(mb_unique<brf::Mesh>{ mesh });
}

void AssetsContoller::add_meshes_with_unique_id(const mb_small_array<MeshTempSettings>& settings)
{
    for (const auto& setting : settings)
    {
        m_meshes.push_back(mb_unique<brf::Mesh>{ setting.m_mesh });
        m_meshes_unique_id[setting.m_name] = setting.m_mesh;
    }
}

brf::Mesh* AssetsContoller::get_mesh(const std::string& name) const
{
    const auto& it = m_meshes_unique_id.find(name);
    return it != m_meshes_unique_id.end() ? it->second : nullptr;
}

std::vector<mb_unique<brf::Mesh>>& AssetsContoller::get_all_meshes()
{
    return m_meshes;
}
