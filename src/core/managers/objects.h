#ifndef _OBJECTS_H
#define _OBJECTS_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include "core/objects/object.h"

class ObjectManager final
{
public:
	template <class _Tx, class... _Args, 
		std::enable_if_t<std::is_base_of_v<Object, _Tx> && !std::is_same_v<Object, _Tx>, int> = 0>
	inline _Tx* create_object(_Args&&... args)
	{
		// Registering an id for object class
		Object::get_static_object_base_id<_Tx>();

		// I don't think this is a good idea to transfer args for ctor
		// I prefer to make a specific method with arguments that i need in a moment
		Unique<_Tx> unique_object = create_unique<_Tx>(std::forward<_Args>(args)...);
		_Tx* object = unique_object.get();

		object->start();
		object->m_id = static_cast<uint32_t>(
			m_objects.size()
		);

		m_objects.push_back(std::move(unique_object));

		return object;
	}

	template <class _Tx,
		std::enable_if_t<std::is_base_of_v<Object, _Tx> && !std::is_same_v<Object, _Tx>, int> = 0>
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

	void remove_object(Object* object);
	void remove_all();

	void draw_all();
private:
	std::vector<Unique<Object>> m_objects;
};

declare_unique_class(ObjectManager, objects)

#endif // !_OBJECTS_H
