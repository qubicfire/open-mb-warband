#ifndef _CONTEXT_H
#define _CONTEXT_H
#include "core/mb_type_traits.h"
#include "vertex_array.h"

namespace mbcore
{
	struct RendererContext
	{
		virtual void draw_vertex_array(VertexArray* vertex_array) = 0;

		static Unique<RendererContext> create();
	};
}

#endif // !_CONTEXT_H
