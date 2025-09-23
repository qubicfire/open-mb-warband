#ifndef _SCENE_H
#define _SCENE_H
#include "core/managers/objects.h"

class Scene
{
	friend class SceneTree;
public:
	virtual void start() { }

	virtual void update_client() { }
	virtual void update() { }

	virtual void dispose() { }
protected:
	void setup();
protected:
	mb_unique<ObjectManager> m_objects;
};

#endif // !_SCENE_H
