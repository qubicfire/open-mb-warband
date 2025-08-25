#ifndef _OPENGL_CONTEXT_H
#define _OPENGL_CONTEXT_H
#include "core/platform/context.h"

struct OpenGLContext final : mbcore::RendererContext
{
	void draw_indexed(mbcore::VertexArray* vertex_array, const uint32_t index_count);
	void draw_triangles(mbcore::VertexArray* vertex_array, const uint32_t triangles);
};

#endif // !_OPENGL_CONTEXT_H
