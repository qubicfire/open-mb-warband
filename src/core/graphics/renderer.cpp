#include "renderer.h"

void Renderer::setup_camera_object(Camera* camera) noexcept
{
	m_camera = camera;
}

void Renderer::update_view_matrix()
{
	m_camera->update_view_matrix();
}

void Renderer::draw_vertex_array(const mb_unique<mbcore::VertexArray>& array)
{
	m_draw_calls++;

	m_context->draw_vertex_array(array.get());
}

void Renderer::dispatch_compute(Shader* shader, 
	const int groups_x, 
	const int groups_y, 
	const int groups_z)
{
	m_context->dispatch_compute(shader, groups_x, groups_y, groups_z);
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
