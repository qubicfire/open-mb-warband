#ifndef _MAP_H
#define _MAP_H
#include "core/objects/model.h"

class Map final : public Model
{
	object_base(Map, Model)
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

	void client_start() override;
	void draw() override;

	glm::vec3 align_point_to_ground(float x, float y);

#ifdef _DEBUG
	bool m_is_debug_enable;
#endif // _DEBUG
protected:
	void bind_all_textures(Shader* shader) const;
	void add_texture(const std::string& path, const mbcore::Texture2D::Type type);
private:
	std::list<mbcore::Texture2D*> m_textures;
	std::list<Unique<mbcore::VertexArray>> m_arrays;
	std::vector<MapVertex> m_vertices;
	std::vector<uint32_t> m_indices;
};

#endif // !_MAP_H
