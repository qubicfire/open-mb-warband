#ifndef _ASSETS_H
#define _ASSETS_H
#include "utils/thread_pool.h"

#include "core/graphics/shader.h"
#include "core/platform/texture2d.h"
#include "brf/resource.h"
#include "brf/mesh.h"

class AssetsContoller final
{
	friend class Scene;
public:
	Shader* load_shader(std::string_view key,
		std::string_view vertex, 
		std::string_view fragment);
	Shader* load_shader(std::string_view key,
		std::string_view path,
		Shader::ShaderType type);

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
	struct MeshTempSettings
	{
		brf::Mesh* m_mesh;
		std::string m_name;
	};
	void add_meshes_with_unique_id(const mb_small_array<MeshTempSettings>& settings);
	brf::Mesh* get_mesh(const std::string& name) const;
	std::vector<mb_unique<brf::Mesh>>& get_all_meshes();

	inline void wait_async_signal()
	{
		g_threads->wait();
	}
private:
	mb_hash_map<std::string_view, mb_unique<Shader>> m_shaders;
	mb_hash_map<std::string, mb_unique<mbcore::Texture2D>> m_textures;
	mb_hash_map<std::string, brf::Mesh*> m_meshes_unique_id;
	std::vector<mb_unique<brf::Mesh>> m_meshes;
	std::vector<mb_unique<brf::Resource>> m_resources;

	static inline std::mutex m_mutex {};
};

create_global_class(AssetsContoller, assets)

#endif // !_ASSETS_H
