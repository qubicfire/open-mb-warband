#ifndef _ASSETS_H
#define _ASSETS_H
#include "utils/thread_pool.h"

#include "core/graphics/shader.h"
#include "core/platform/texture2d.h"
#include "brf/resource.h"
#include "brf/mesh.h"

class AssetsContoller final
{
public:
	Shader* load_shader(std::string_view key,
		std::string_view vertex, 
		std::string_view fragment);

	Shader* get_shader(std::string_view key) const;
	void remove_shader(std::string_view key);

	brf::Resource* load_resource(const std::string& path);
	inline void load_resource_async(const std::string& path)
	{
		g_threads->detach_task([this, path]() { load_resource(path); });
	}

	mbcore::Texture2D* load_texture(const std::string& path, 
		const mbcore::Texture2D::Type type);

	void add_mesh(brf::Mesh* mesh);
	brf::Mesh* get_mesh(const std::string& name) const;

	inline void wait_async_signal()
	{
		g_threads->wait();
	}
private:
	mb_hash_map<std::string_view, Unique<Shader>> m_shaders;
	mb_hash_map<std::string, Unique<mbcore::Texture2D>> m_textures;
	mb_hash_map<std::string, brf::Mesh*> m_meshes;
	std::vector<Unique<brf::Resource>> m_resources;

	static inline std::mutex m_mutex {};
};

create_global_class(AssetsContoller, assets)

#endif // !_ASSETS_H
