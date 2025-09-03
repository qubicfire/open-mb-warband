#ifndef _MAP_H
#define _MAP_H
#include "core/objects/model.h"
#include "core/objects/rigidbody.h"

class Map final : public Model
{
	object_base(Map)
public:
	struct MapVertex
	{
		glm::vec3 m_origin;
		glm::vec2 m_texture;
#ifdef _DEBUG
		glm::vec3 m_color;
#endif // _DEBUG
		float m_type;
		bool m_is_done;
	};

	void start_client() override;
	void draw() override;

	glm::vec3 align_point_to_ground(float x, float y);

#ifdef _DEBUG
	bool m_is_debug_enable;
#endif // _DEBUG
protected:
	void bind_all_textures(Shader* shader) const;
	void add_texture(const std::string& path, const mbcore::Texture2D::Type type);
private:
	RigidBody m_body;

	std::vector<mbcore::Texture2D*> m_textures;
	std::vector<Unique<mbcore::VertexArray>> m_arrays;
	std::vector<glm::vec3> m_vertices;
	std::vector<uint32_t> m_indices;
};

#endif // !_MAP_H
