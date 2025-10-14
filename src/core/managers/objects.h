#ifndef _OBJECTS_H
#define _OBJECTS_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include "core/objects/object.h"
#include "core/graphics/shader.h"

class ObjectManager final
{
	friend class Object;
	friend class ObjectFactory;
	friend class Scene;
public:
	void initialize();

	template <class _Tx, class... _Args, 
		std::enable_if_t<std::is_base_of_v<Object, _Tx> && 
		!std::is_same_v<Object, _Tx>, int> = 0>
	inline _Tx* create_object(_Args&&... args)
	{
		// Registering an id for object class
		Object::get_static_object_base_id<_Tx>();

		// I don't think it's a good idea to pass args for ctor
		// I prefer to make a specific method with arguments that i need in a moment
		mb_unique<_Tx> unique_object = create_unique<_Tx>(std::forward<_Args>(args)...);
		_Tx* object = unique_object.get();

		object->start_internal();
		object->m_id = static_cast<uint32_t>(
			m_objects.size()
		);

		m_objects.push_back(std::move(unique_object));

		return object;
	}

	template <class _Tx,
		std::enable_if_t<std::is_base_of_v<Object, _Tx> && 
		!std::is_same_v<Object, _Tx>, int> = 0>
	inline std::vector<_Tx*> find_all()
	{
		std::vector<_Tx*> objects {};
		const uint16_t type = Object::get_static_object_base_id<_Tx>();

		for (const auto& unique_object : m_objects)
		{
			if (unique_object->get_object_base_id() == type)
			{
				_Tx* object = static_cast<_Tx*>(
					unique_object.get()
				);

				objects.push_back(object);
			}
		}

		return objects;
	}

	template <class _Tx,
		std::enable_if_t<std::is_base_of_v<Object, _Tx> &&
		!std::is_same_v<Object, _Tx>, int> = 0>
	inline _Tx* find()
	{
		const uint16_t type = Object::get_static_object_base_id<_Tx>();

		for (const auto& unique_object : m_objects)
		{
			if (unique_object->get_object_base_id() == type)
			{
				_Tx* object = static_cast<_Tx*>(
					unique_object.get()
				);

				return object;
			}
		}

		return nullptr;
	}

	template <class _Tx = Object>
	inline _Tx* find(const uint32_t index)
	{
		const mb_unique<Object>& object = m_objects[index];

		return static_cast<_Tx*>(
			object.get()
		);
	}

	void remove_object(Object* object);
	void remove_all();

	void update_all();
	void draw_all();

#ifdef _DEBUG
	void draw_aabb(const mb_unique<Object>& object);

	static inline bool m_is_aabb_enabled = true;
#endif // _DEBUG
private:
	void add_object(Object* object)
	{
		mb_unique<Object> unique_object{ object };

		object->start_internal();
		object->m_id = static_cast<uint32_t>(
			m_objects.size()
		);

		m_objects.push_back(std::move(unique_object));
	}
private:
	Shader* m_culling_shader;
	std::vector<mb_unique<Object>> m_objects;
};

declare_global_class(ObjectManager, objects)

class ObjectFactory
{
public:
	using ObjectInstantiate = Object*(*)();

	explicit ObjectFactory(std::string_view name, ObjectInstantiate instantiate)
	{
		m_factories.emplace(name, instantiate);
	}

	static Object* instantiate(std::string_view name)
	{
		Object* object = m_factories[name]();
		g_objects->add_object(object);

		return object;
	}
private:
	static inline mb_hash_map<std::string_view, ObjectInstantiate> m_factories {};
};

#define bind_object_factory(name, type)															\
	static inline Object* s__ObjectInstantiate_ ##name() { return new type(); }					\
	static inline ObjectFactory s__ObjectFactory_ ##name(#name, s__ObjectInstantiate_ ##name);	\

#endif // !_OBJECTS_H
