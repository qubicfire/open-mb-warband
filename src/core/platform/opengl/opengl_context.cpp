#include "opengl.h"

#include "opengl_context.h"

void OpenGLContext::draw_indexed(mbcore::VertexArray* vertex_array, const uint32_t index_count)
{
	glBindVertexArray(vertex_array->get_id());
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
}
