#include "core/graphics/renderer.h"
#include "core/managers/assets.h"
#include "utils/assert.h"

#include "shader.h"
#include "core/platform/opengl/opengl_shader.h"

const uint32_t Shader::get_id() const noexcept
{
    return m_id;
}

mb_unique<Shader> Shader::create(std::string_view vertex, std::string_view fragment)
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLShader>(vertex, fragment);
	}

	core_assert_immediatly("%s", "Unable to create a shader. Renderer API is invalid");
	return nullptr;
}

Shader* Shader::get(std::string_view key)
{
	return g_assets->get_shader(key);
}
