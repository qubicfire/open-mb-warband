#ifndef _OPENGL_CONTEXT_H
#define _OPENGL_CONTEXT_H
#include "core/platform/context.h"

struct OpenGLContext : mbcore::RendererContext
{
	void draw_indexed(mbcore::VertexArray* vertex_array, const uint32_t index_count);
};

#endif // !_OPENGL_CONTEXT_H
