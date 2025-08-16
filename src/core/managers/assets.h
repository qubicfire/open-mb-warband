#ifndef _ASSETS_H
#define _ASSETS_H
#include "utils/thread_pool.h"

#include "core/graphics/shader.h"
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

	void add_mesh(brf::Mesh* mesh);
	brf::Mesh* get_mesh(const std::string& name) const;

	inline void wait_async_signal()
	{
		g_threads->wait();
	}
private:
	HashMap<std::string_view, Unique<Shader>> m_shaders;
	HashMap<std::string, brf::Mesh*> m_meshes;
	std::vector<Unique<brf::Resource>> m_resources;

	static inline std::mutex m_mutex {};
};

create_global_class(AssetsContoller, assets)

#endif // !_ASSETS_H
