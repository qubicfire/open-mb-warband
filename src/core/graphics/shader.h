#ifndef _SHADER_H
#define _SHADER_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include <glm/ext/matrix_float4x4.hpp>

class Shader
{
public:
	virtual void load(std::string_view vertex, std::string_view fragment) = 0;

	virtual void bind() const = 0;

	virtual void set_bool(const char* location, const bool value) const = 0;
	virtual void set_int(const char* location, const int value) const = 0;
	virtual void set_float(const char* location, const float value) const = 0;
	virtual void set_vec2(const char* location, const glm::vec2& v) const = 0;
	virtual void set_vec3(const char* location, const glm::vec3& v) const = 0;
	virtual void set_vec4(const char* location, const glm::vec4& v) const = 0;
	virtual void set_mat4(const char* location, const glm::mat4& mat) const = 0;

	const uint32_t get_id() const noexcept;

	static mb_unique<Shader> create(std::string_view vertex, std::string_view fragment);
	static Shader* get(std::string_view key);
protected:
	uint32_t m_id;
};

#endif // !_SHADER_H
