#include "core/managers/objects.h"
#include "core/managers/assets.h"

#include "scene.h"

void Scene::setup()
{
	if (!m_objects)
		m_objects = create_unique<ObjectManager>();

	g_objects = m_objects.get();

	if (Client::is_running())
		g_objects->initialize();
}
