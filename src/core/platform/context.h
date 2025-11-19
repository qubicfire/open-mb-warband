#ifndef _CONTEXT_H
#define _CONTEXT_H
#include "core/mb_type_traits.h"
#include "core/graphics/shader.h"
#include "vertex_array.h"

namespace mbcore
{
	struct RendererContext
	{
		virtual void draw_vertex_array(const mb_unique<mbcore::VertexArray>& vertex_array) = 0;
		virtual void dispatch_compute(Shader* shader,
			const int groups_x,
			const int groups_y,
			const int groups_z) = 0;

		static mb_unique<RendererContext> create();
	};
}

#endif // !_CONTEXT_H
