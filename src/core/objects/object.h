#ifndef _OBJECT_H
#define _OBJECT_H
#include <glm/ext/vector_float3.hpp>

#include "core/mb.h"
#include "core/graphics/shader.h"

#include "core/net/packet.h"

#define object_base(type)									   \
public:															   \
	virtual inline uint16_t get_object_base_id() const noexcept	   \
	{															   \
		return Object::get_static_object_base_id<type>();		   \
	}															   \

#define object_client_base(shader_name)							   \
	virtual const char* get_shader_name() { return #shader_name; } \

class Object : public NetworkListener
{
	friend class ObjectManager;

	object_base(Object)
public:
	virtual void start() {}
	virtual void start_client() {}

	virtual void update() {}
	virtual void draw() {}

	void set_origin(const glm::vec3& origin) noexcept;
	void set_rotation(const glm::vec3& rotation) noexcept;
	void set_scale(const glm::vec3& scale) noexcept;
	void set_angle(const float angle) noexcept;

	const glm::vec3& get_origin() const noexcept;
	const glm::vec3& get_rotation() const noexcept;
	const glm::vec3& get_scale() const noexcept;
	const float get_angle() const noexcept;
	const uint32_t get_id() const noexcept;

	template <class _Tx, class... _Args,
		std::enable_if_t<std::is_base_of_v<Object, _Tx>, int> = 0>
	static inline _Tx* instantiate(_Args&&... args)
	{
		_Tx* object = new _Tx(std::forward<_Args>(args)...);
		Object::get_static_object_base_id<_Tx>();

		instantiate_internal(object);

		return object;
	}
protected:
	void start_internal();
	virtual void draw_internal(Shader* shader) { }

	// virtual const char* get_shader_name() { }

	static inline uint16_t get_static_object_base_id_impl() noexcept
	{
		static uint16_t id;
		return id++;
	}

	template <class _Tx, 
		std::enable_if_t<std::is_base_of_v<Object, _Tx>, int> = 0>
	static inline uint16_t get_static_object_base_id() noexcept
	{
		static uint16_t id = get_static_object_base_id_impl();
		return id;
	}
private:
	static void instantiate_internal(Object* object);
protected:
	glm::vec3 m_origin;
	glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 m_scale = glm::vec3(1.0f);
	float m_angle;
private:
	uint32_t m_id;
};

#endif // !_OBJECT_H
