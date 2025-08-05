#include "assets.h"

Shader* AssetsContoller::load_shader(std::string_view key, 
    std::string_view vertex, 
    std::string_view fragment)
{
    Unique<Shader> shader_unique = Shader::create(vertex, fragment);
    Shader* shader = shader_unique.get();

    m_shaders.emplace(key, std::move(shader_unique));

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
    Unique<brf::Resource> resource_unique = create_unique<brf::Resource>();

    if (!resource_unique->load(path))
    {
        log_alert("Failed to load \'%s\' resource", path.c_str());
        return nullptr;
    }
    else
    {
        log_print("Resource file \'%s\' loaded\n\t[%s]:\n\t[Meshes]: %d",
            path.c_str(),
            path.c_str(),
            resource_unique->get_meshes_count());

        brf::Resource* resource = resource_unique.get();

        // Where's std version of concurrent vector or something like that?
        {
            std::lock_guard lock(m_mutex);
            m_resources.push_back(std::move(resource_unique));
        }

        return resource;
    }
}

void AssetsContoller::add_mesh(brf::Mesh* mesh)
{
    m_meshes[mesh->get_name()] = mesh;
}

brf::Mesh* AssetsContoller::get_mesh(const std::string& name) const
{
    const auto& it = m_meshes.find(name);
    return it != m_meshes.end() ? it->second : nullptr;
}
