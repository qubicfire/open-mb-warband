#include "core/graphics/renderer.h"
#include "utils/assert.h"
#include "vertex_array.h"
#include "core/platform/opengl/opengl_vertex_array.h"

using namespace mbcore;

void VertexArray::set_vertex_buffer(mb_unique<VertexBuffer>& buffer)
{
	m_vertex_buffer = std::move(buffer);
}

void VertexArray::set_index_buffer(mb_unique<IndexBuffer>& buffer)
{
	m_index_buffer = std::move(buffer);
}

const VertexFlags VertexArray::get_flags() const
{
	return m_flags;
}

const uint32_t VertexArray::get_id() const
{
	return m_id;
}

VertexBuffer* VertexArray::get_vertex_buffer() const
{
	return m_vertex_buffer.get();
}

IndexBuffer* VertexArray::get_index_buffer() const
{
	return m_index_buffer.get();
}

mb_unique<VertexArray> VertexArray::create(int flags)
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLVertexArray>(flags);
	}

	core_assert_immediatly("%s", "Unable to create a index buffer. Renderer API is invalid");
	return nullptr;
}
