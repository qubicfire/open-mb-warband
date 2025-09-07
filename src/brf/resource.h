#ifndef _BFR_RESOURCE_H
#define _BFR_RESOURCE_H
#include "mesh.h"
#include "body.h"
#include "texture.h"
#include "material.h"
#include "skeleton.h"
#include "animation.h"

namespace brf
{
	class Resource
	{
	public:
		bool load(const std::string& path);

		Mesh* get_mesh(const std::string& name);
		mb_small_array<Mesh>& get_meshes();
		const uint32_t get_meshes_count() const;
	private:
		mb_small_array<Mesh> m_meshes;
		mb_small_array<Body> m_bodies;
		mb_small_array<Texture> m_textures;
		mb_small_array<Skeleton> m_skeletons;
		mb_small_array<Animation> m_animations;
		mb_small_array<Material> m_materials;
	};
}

#endif // !_BFR_RESOURCE_H
