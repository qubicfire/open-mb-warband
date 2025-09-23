#include <fstream>
#include "opengl.h"

#include "opengl_shader.h"

static const std::array<uint32_t, Shader::LastShader> SHADER_TYPES =
{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_COMPUTE_SHADER
};

OpenGLShader::OpenGLShader(std::string_view vertex, std::string_view fragment)
{
	load(vertex, fragment);
}

OpenGLShader::OpenGLShader(std::string_view path, ShaderType type)
{
	load(path, type);
}

void OpenGLShader::load(std::string_view vertex, std::string_view fragment)
{
	const uint32_t vertex_shader = load_shader_part(vertex, GL_VERTEX_SHADER);
	const uint32_t fragment_shader = load_shader_part(fragment, GL_FRAGMENT_SHADER);

	m_id = glCreateProgram();
	glAttachShader(m_id, vertex_shader);
	glAttachShader(m_id, fragment_shader);
	glLinkProgram(m_id);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void OpenGLShader::load(std::string_view path, ShaderType type)
{
	const uint32_t shader_type = SHADER_TYPES[type];
	const uint32_t shader = load_shader_part(path, shader_type);

	m_id = glCreateProgram();
	glAttachShader(m_id, shader);
	glLinkProgram(m_id);

	glDeleteShader(shader);
}

void OpenGLShader::bind() const
{
	glUseProgram(m_id);
}

void OpenGLShader::set_bool(const char* location, bool value) const
{
	const uint32_t id = glGetUniformLocation(m_id, location);
	glUniform1i(id, static_cast<int>(value));
}

void OpenGLShader::set_int(const char* location, const int value) const
{
	const uint32_t id = glGetUniformLocation(m_id, location);
	glUniform1i(id, value);
}

void OpenGLShader::set_float(const char* location, const float value) const
{
	const uint32_t id = glGetUniformLocation(m_id, location);
	glUniform1f(id, value);
}

void OpenGLShader::set_vec2(const char* location, const glm::vec2& v) const
{
	const uint32_t id = glGetUniformLocation(m_id, location);
	glUniform2fv(id, 1, glm::value_ptr(v));
}

void OpenGLShader::set_vec3(const char* location, const glm::vec3& v) const
{
	const uint32_t id = glGetUniformLocation(m_id, location);
	glUniform3fv(id, 1, glm::value_ptr(v));
}

void OpenGLShader::set_vec4(const char* location, const glm::vec4& v) const
{
	const uint32_t id = glGetUniformLocation(m_id, location);
	glUniform4fv(id, 1, glm::value_ptr(v));
}

void OpenGLShader::set_mat4(const char* location, const glm::mat4& m) const
{
	const uint32_t id = glGetUniformLocation(m_id, location);
	glUniformMatrix4fv(id, 1, false, glm::value_ptr(m));
}

uint32_t OpenGLShader::load_shader_part(std::string_view name, const uint32_t type)
{
	std::ifstream stream(name.data(), std::ios::in);
	std::string source = { std::istreambuf_iterator<char>(stream),
		std::istreambuf_iterator<char>() };

	uint32_t shader = glCreateShader(type);
	const char* c = source.c_str();
	glShaderSource(shader, 1, &c, nullptr);
	glCompileShader(shader);

	int success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char message[512];
		glGetShaderInfoLog(shader, sizeof(message), nullptr, message);

		log_alert(message);

		return std::numeric_limits<uint32_t>::max();
	}

	return shader;
}
