#include "core/graphics/renderer.h"
#include "core/graphics/text_builder_3d.h"

#include "text_3d.h"

void Text3D::draw()
{
	static Shader* shader = Shader::get("text_3d");
	draw_internal(shader);
}

void Text3D::set_text(const std::string& text, const float scale)
{
	m_vertex_array = g_text_builder_3d->construct(text);

	m_scale = glm::vec3(scale);
}

void Text3D::draw_internal(Shader* shader)
{
	Renderer::build_model_projection_only(shader, m_origin, m_rotation, m_scale, m_angle);

	Camera* camera = Renderer::get_camera();
	shader->set_vec3("u_camera_origin", camera->get_origin());
	shader->set_vec3("u_camera_up", camera->get_up());

	g_text_builder_3d->get_texture()->bind();

	shader->set_int("u_texture", 0);

	Renderer::draw_vertex_array(m_vertex_array);
}
