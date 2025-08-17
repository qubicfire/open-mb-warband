#ifndef _SCENE_H
#define _SCENE_H
#include "core/managers/objects.h"

class Scene
{
	friend class SceneTree;
public:
	virtual void start() { }

	virtual void client_update() { }
	virtual void update() { }

	virtual void dispose() { }
protected:
	// Is there a possible way to put this shit into Object class to make it pretty?
	struct GameObject
	{
		template <class _Tx, class... _Args,
			std::enable_if_t<std::is_base_of_v<Object, _Tx>, int> = 0>
		static inline _Tx* instantiate(_Args&&... args)
		{
			return g_objects->create_object<_Tx>(std::forward<_Args>(args)...);
		}
	};

	void setup();
protected:
	Unique<ObjectManager> m_objects;
};

#endif // !_SCENE_H
