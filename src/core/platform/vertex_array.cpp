#include "core/graphics/renderer.h"
#include "utils/assert.h"
#include "vertex_array.h"
#include "core/platform/opengl/opengl_vertex_array.h"

using namespace mbcore;

void VertexArray::set_vertex_buffer(Unique<VertexBuffer>& buffer)
{
	m_vertex_buffer = std::move(buffer);
}

void VertexArray::set_index_buffer(Unique<IndexBuffer>& buffer)
{
	m_index_buffer = std::move(buffer);
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

Unique<VertexArray> VertexArray::create()
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLVertexArray>();
	}

	core_assert_immediatly("%s", "Unable to create a index buffer. Renderer API is invalid");
	return nullptr;
}
