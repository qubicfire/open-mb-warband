#ifndef _OPENGL_CONTEXT_H
#define _OPENGL_CONTEXT_H
#include "core/platform/context.h"

struct OpenGLContext final : mbcore::RendererContext
{
	void draw_vertex_array(mbcore::VertexArray* vertex_array) override;
};

#endif // !_OPENGL_CONTEXT_H
