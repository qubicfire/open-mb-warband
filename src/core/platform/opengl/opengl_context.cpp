#include "opengl.h"

#include "opengl_context.h"

using namespace mbcore;

void OpenGLContext::draw_vertex_array(VertexArray* vertex_array)
{
	glBindVertexArray(vertex_array->get_id());

	switch (vertex_array->get_flags())
	{
	case VertexFlags::Triangles:
		glDrawArrays(GL_TRIANGLES,
			0,
			vertex_array->get_vertex_buffer()->m_count);
		break;
	case VertexFlags::Indexes:
		glDrawElements(GL_TRIANGLES, 
			vertex_array->get_index_buffer()->m_count,
			GL_UNSIGNED_INT,
			nullptr);
		break;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
