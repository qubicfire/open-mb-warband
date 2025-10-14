#include "core/graphics/renderer.h"
#include "utils/assert.h"
#include "buffer.h"
#include "core/platform/opengl/opengl_buffer.h"

using namespace mbcore;

mb_unique<Buffer> Buffer::create(const void* vertices,
	const size_t count,
	const size_t size,
	const Buffer::Types flags)
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLBuffer>(vertices, count, size, flags);
	}

	core_assert_immediatly("%s", "Unable to create a vertex buffer. Renderer API is invalid");
	return nullptr;
}
