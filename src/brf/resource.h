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
		std::vector<Mesh>& get_meshes();
		const uint32_t get_meshes_count() const;
	private:
		std::vector<Mesh> m_meshes;
		std::vector<Body> m_bodies;
		std::vector<Texture> m_textures;
		std::vector<Skeleton> m_skeletons;
		std::vector<Animation> m_animations;
		std::vector<Material> m_materials;
	};
}

#endif // !_BFR_RESOURCE_H
