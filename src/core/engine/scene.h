#ifndef _SCENE_H
#define _SCENE_H
#include "core/managers/objects.h"
#include "core/managers/nav_mesh.h"

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
	void replace_globals();
protected:
	mb_unique<ObjectManager> m_objects;
	mb_unique<NavMesh> m_nav_mesh;
};

#endif // !_SCENE_H
