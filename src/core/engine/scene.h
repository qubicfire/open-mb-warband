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
	void setup();
protected:
	Unique<ObjectManager> m_objects;
};

#endif // !_SCENE_H
