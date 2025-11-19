#ifndef _OPENGL_CONTEXT_H
#define _OPENGL_CONTEXT_H
#include "core/platform/context.h"

struct OpenGLContext final : mbcore::RendererContext
{
	void draw_vertex_array(const mb_unique<mbcore::VertexArray>& vertex_array) override;
	void dispatch_compute(Shader* shader,
		const int groups_x,
		const int groups_y,
		const int groups_z) override;
};

#endif // !_OPENGL_CONTEXT_H
