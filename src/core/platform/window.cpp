#include "core/graphics/renderer.h"
#include "core/engine/engine.h"

#include "window.h"

#include "utils/assert.h"
#include "opengl/opengl_window.h"

using namespace mbcore;

mb_unique<Window> Window::create(const WindowProperties& properties)
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLWindow>(properties);
	}

	core_assert_immediatly("%s", "Unable to create a window. Renderer API is invalid");
	return nullptr; // wtf?
}

mb_unique<Platform> Platform::create()
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLPlatform>();
	}

	core_assert_immediatly("%s", "Unable to create a platform. Platform is invalid");
	return nullptr;
}
