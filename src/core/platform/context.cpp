#include "core/graphics/renderer.h"
#include "utils/assert.h"
#include "context.h"
#include "opengl/opengl_context.h"

using namespace mbcore;

Unique<RendererContext> RendererContext::create()
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLContext>();
	}

	core_assert_immediatly("%s", "Unable to create a renderer context. Renderer context is invalid");
	return nullptr; // wtf?
}
