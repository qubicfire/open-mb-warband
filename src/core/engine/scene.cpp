#include "core/managers/objects.h"

#include "scene.h"

void Scene::setup()
{
	if (!m_objects)
		m_objects = create_unique<ObjectManager>();

	g_objects = m_objects.get();
}
