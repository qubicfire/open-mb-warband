#include "renderer.h"

void Renderer::setup_camera_object(Camera* camera) noexcept
{
	m_camera = camera;
}

void Renderer::build_model_projection(Shader* shader,
	const glm::vec3& origin,
	const glm::vec3& rotation,
	const glm::vec3& scale,
	const float angle)
{
	shader->bind();

	shader->set_mat4("u_view", m_camera->get_view());
	shader->set_mat4("u_projection", m_camera->get_projection());
	shader->set_vec3("u_light_origin", m_camera->get_origin());
	shader->set_vec3("u_light_color", glm::vec3(1.0f, 1.0f, 1.0f));

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, origin);
	transform = glm::rotate(transform, glm::radians(angle), rotation);
	transform = glm::scale(transform, scale);
	shader->set_mat4("u_model", transform);
}

void Renderer::build_model_projection_only(Shader* shader,
	const glm::vec3& origin,
	const glm::vec3& rotation, 
	const glm::vec3& scale, 
	const float angle)
{
	shader->bind();

	shader->set_mat4("u_view", m_camera->get_view());
	shader->set_mat4("u_projection", m_camera->get_projection());

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, origin);
	transform = glm::rotate(transform, glm::radians(angle), rotation);
	transform = glm::scale(transform, scale);
	shader->set_mat4("u_model", transform);
}

void Renderer::update_view_matrix()
{
	m_camera->update_view_matrix();
}

void Renderer::draw_indexed(const Unique<mbcore::VertexArray>& array)
{
	m_draw_calls++;

	m_context->draw_indexed(array.get(), array->get_index_buffer()->m_count);

	array->unbind();
}

void Renderer::draw_triangles(const Unique<mbcore::VertexArray>& array)
{
	m_draw_calls++;

	m_context->draw_triangles(array.get(), array->get_vertex_buffer()->m_count);

	array->unbind();
}

void Renderer::reset()
{
	m_draw_calls = 0;
}

#ifdef _DEBUG
Camera* Renderer::get_camera()
{
	return m_camera;
}
#endif	

uint32_t Renderer::get_draw_calls()
{
	return m_draw_calls;
}
