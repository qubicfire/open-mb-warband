#ifndef _OPENGL_SHADER_H
#define _OPENGL_SHADER_H
#include "core/graphics/shader.h"

class OpenGLShader : public Shader
{
public:
	OpenGLShader(std::string_view vertex, std::string_view fragment);
	OpenGLShader(std::string_view path, ShaderType type);

	void load(std::string_view vertex, std::string_view fragment);
	void load(std::string_view path, ShaderType type);

	void bind() const;

	void set_bool(const char* location, const bool value) const;
	void set_int(const char* location, const int value) const;
	void set_float(const char* location, const float value) const;
	void set_vec2(const char* location, const glm::vec2& v) const;
	void set_vec3(const char* location, const glm::vec3& v) const;
	void set_vec4(const char* location, const glm::vec4& v) const;
	void set_mat4(const char* location, const glm::mat4& mat) const;
private:
	uint32_t load_shader_part(std::string_view name, const uint32_t type);
};

#endif // !_OPENGL_SHADER_H
