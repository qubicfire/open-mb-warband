#ifndef _CONTEXT_H
#define _CONTEXT_H
#include "core/mb_type_traits.h"
#include "vertex_array.h"

namespace mbcore
{
	struct RendererContext
	{
		virtual void draw_indexed(VertexArray* vertex_array, const uint32_t index_count) = 0;
		virtual void draw_triangles(VertexArray* vertex_array, const uint32_t triangles) = 0;
		//virtual void draw_quads(VertexArray* vertex_array, const uint32_t quads) = 0;

		static Unique<RendererContext> create();
	};
}

#endif // !_CONTEXT_H
