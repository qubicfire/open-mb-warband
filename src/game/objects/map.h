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
		float m_type = -1.0f;
	};

	void client_start() override;
	void draw() override;

	glm::vec3 align_point_to_ground(float x, float y);
protected:
	void bind_all_textures(Shader* shader) const;
	void add_texture(const std::string& path, const mbcore::Texture2D::Type type);
private:
	std::list<mbcore::Texture2D*> m_textures;
	std::vector<MapVertex> m_vertices;
	std::vector<uint32_t> m_indices;
};

#endif // !_MAP_H
