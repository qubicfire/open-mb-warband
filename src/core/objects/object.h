#ifndef _OBJECT_H
#define _OBJECT_H
#include "core/mb.h"
#include "core/graphics/shader.h"
#include "utils/mb_bit_set.h"

#include "core/net/server.h"
#include "brf/mesh.h"
#include "core/platform/texture2d.h"

#define object_base(type)										      \
public:															      \
	virtual inline uint16_t get_object_base_id() const noexcept	      \
	{															      \
		return Object::get_static_object_base_id<type>();		      \
	}															      \

#define object_client_base(shader_name)							      \
private:															  \
	virtual Shader* get_shader()									  \
	{																  \
		static Shader* instance = Shader::get(#shader_name);		  \
		return instance;										      \
	}																  \
	virtual std::string_view get_shader_view()						  \
	{																  \
		return std::string_view(#shader_name);						  \
	}																  \

class Object
{
	template <class _Tx, auto _Kx>
	friend class NetworkField;

	friend class ObjectManager;

	enum class ObjectNetworkEvent : int8_t
	{
		Origin,
		Rotation,
		Scale,
		LastEvent
	};

	object_base(Object)
	object_client_base(main)
public:
	enum class ObjectFlags : int8_t
	{
		Invisible = (1 << 0),
		TextureArrayOld = (1 << 1),
		GlobalTime = (1 << 2),
		DirtyMatrix = (1 << 3),
	};

	friend inline ObjectFlags operator|(ObjectFlags left, ObjectFlags right) 
	{
		return static_cast<ObjectFlags>(
			static_cast<std::underlying_type_t<ObjectFlags>>(left) |
			static_cast<std::underlying_type_t<ObjectFlags>>(right)
		);
	}

	virtual void start() { }
	virtual void start_client() { }

	virtual void update() { }

	void add_mesh(brf::Mesh* mesh);
	void add_texture(mbcore::Texture2D* texture);
	void add_texture(const std::string& path, const mbcore::Texture2D::Type type);
	void add_child(Object* child);

	void set_object_flag(const ObjectFlags flags);
	void set_object_flags(const ObjectFlags flags);
	void set_aabb(const glm::vec3& min, const glm::vec3& max);
	void set_parent(Object* parent);
	void set_origin(const glm::vec3& origin);
	void set_rotation(const glm::vec3& rotation);
	void set_scale(const glm::vec3& scale);
	void set_angle(const float angle);

	virtual void server_send_packet() { }
	virtual void client_send_packet() { }

	virtual void server_receive_packet(uint8_t*) { }
	virtual void client_receive_packet(uint8_t*) { }

	bool has_network_state_changed() const
	{
		return m_network_state > 0;
	}

	const glm::mat4& get_transform();
	const mb_bit_set<ObjectFlags> get_object_flags() const;
	const AABB& get_aabb() const;
	const glm::vec3& get_origin() const;
	const glm::vec3& get_rotation() const;
	const glm::vec3& get_scale() const;
	const float get_angle() const;
	const uint32_t get_id() const;
	brf::Mesh* get_mesh() const;
	std::list<brf::Mesh*>& get_meshes();
	mbcore::Texture2D* get_texture() const;
	std::list<mbcore::Texture2D*>& get_textures();

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
	
	std::list<brf::Mesh*> m_meshes;
	AABB m_aabb;
private:
	glm::mat4 m_transform = glm::mat4(1.0f);
	std::list<Object*> m_childs;
	std::list<mbcore::Texture2D*> m_textures;
	mb_bit_set<ObjectFlags> m_flags;
	int m_network_state;
	uint32_t m_id;
};

#endif // !_OBJECT_H
