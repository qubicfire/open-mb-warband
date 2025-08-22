#include "core/graphics/renderer.h"
#include "core/managers/assets.h"

#include "model.h"

using namespace mbcore;

void Model::draw()
{
	static Shader* shader = g_assets->get_shader("main");
	
	draw_internal(shader);
}

VertexArray* Model::get_vertex_array() const
{
    return m_vertex_array.get();
}

void Model::draw_internal(Shader* shader)
{
	Renderer::prepare_model_projection(shader, m_origin, m_rotation, m_scale, m_angle);

	bind_all_textures(shader);

	Renderer::draw_indexed(m_vertex_array);
}
