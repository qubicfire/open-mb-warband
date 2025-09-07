#include "core/graphics/renderer.h"

#include "model.h"

using namespace mbcore;

void Model::draw()
{
	static Shader* shader = Shader::get("main");
	
	draw_internal(shader);
}

VertexArray* Model::get_vertex_array() const
{
    return m_vertex_array.get();
}

void Model::draw_internal(Shader* shader)
{
	Renderer::build_model_projection(shader, m_origin, m_rotation, m_scale, m_angle);

	bind_all_textures(shader);

	Renderer::draw_vertex_array(m_vertex_array);
}
