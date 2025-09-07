#include "core/graphics/renderer.h"
#include "utils/assert.h"
#include "buffer.h"
#include "core/platform/opengl/opengl_buffer.h"

using namespace mbcore;

Unique<VertexBuffer> VertexBuffer::create(const void* vertices,
	const size_t count,
	const size_t size,
	int flags)
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLVertexBuffer>(vertices, count, size, flags);
	}

	core_assert_immediatly("%s", "Unable to create a vertex buffer. Renderer API is invalid");
	return nullptr;
}

Unique<IndexBuffer> IndexBuffer::create(const uint32_t* indices, const size_t size)
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLIndexBuffer>(indices, size);
	}

	core_assert_immediatly("%s", "Unable to create a index buffer. Renderer API is invalid");
	return nullptr;
}
