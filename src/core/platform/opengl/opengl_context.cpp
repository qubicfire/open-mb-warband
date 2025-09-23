#include "opengl.h"

#include "opengl_context.h"

using namespace mbcore;

void OpenGLContext::draw_vertex_array(VertexArray* vertex_array)
{
	glBindVertexArray(vertex_array->get_id());

	switch (vertex_array->get_flags())
	{
	case RendererType::Triangles:
		glDrawArrays(GL_TRIANGLES,
			0,
			vertex_array->get_vertex_buffer()->m_count);
		break;
	case RendererType::Indexes:
		glDrawElements(GL_TRIANGLES, 
			vertex_array->get_index_buffer()->m_count,
			GL_UNSIGNED_INT,
			nullptr);
		break;
	case RendererType::Indirect:
		break;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLContext::dispatch_compute(Shader* shader, 
	const int groups_x,
	const int groups_y,
	const int groups_z)
{
	glUseProgram(shader->get_id());

	glDispatchCompute(groups_x, groups_y, groups_z);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
