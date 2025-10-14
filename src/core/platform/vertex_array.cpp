#include "core/graphics/renderer.h"
#include "utils/assert.h"
#include "vertex_array.h"
#include "core/platform/opengl/opengl_vertex_array.h"

using namespace mbcore;

void VertexArray::set_vertex_buffer(mb_unique<Buffer>& buffer, int index)
{
	m_vertex_buffers[index] = std::move(buffer);
}

void VertexArray::set_index_buffer(mb_unique<Buffer>& buffer)
{
	m_index_buffer = std::move(buffer);
}

const VertexArray::Types VertexArray::get_flags() const
{
	return m_flags;
}

const uint32_t VertexArray::get_id() const
{
	return m_id;
}

Buffer* VertexArray::get_vertex_buffer(int index) const
{
	return m_vertex_buffers[index].get();
}

mb_small_array<mb_unique<Buffer>>& VertexArray::get_vertex_buffers()
{
	return m_vertex_buffers;
}

Buffer* VertexArray::get_index_buffer() const
{
	return m_index_buffer.get();
}

mb_unique<VertexArray> VertexArray::create(int buffers,
	const Types flags)
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLVertexArray>(buffers, flags);
	}

	core_assert_immediatly("%s", "Unable to create a index buffer. Renderer API is invalid");
	return nullptr;
}
