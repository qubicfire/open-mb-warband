#include "opengl.h"

#include "opengl_context.h"

void OpenGLContext::draw_indexed(mbcore::VertexArray* vertex_array, 
	const uint32_t index_count)
{
	glBindVertexArray(vertex_array->get_id());
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
}

void OpenGLContext::draw_triangles(mbcore::VertexArray* vertex_array,
	const uint32_t triangles)
{
	glBindVertexArray(vertex_array->get_id());
	glDrawArrays(GL_TRIANGLES, 0, triangles);
}

//void OpenGLContext::draw_quads(mbcore::VertexArray* vertex_array,
//	const uint32_t quads)
//{
//	glBindVertexArray(vertex_array->get_id());
//	glDrawArrays(GL_QUADS, 0, quads);
//}
