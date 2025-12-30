#include "core/managers/objects.h"
#include "core/managers/assets.h"

#include "scene.h"

void Scene::setup()
{
	m_objects = create_unique<ObjectManager>();
	m_nav_mesh = create_unique<NavMesh>();

	replace_globals();

	if (Client::is_running())
		g_objects->initialize();
}

void Scene::replace_globals()
{
	g_objects = m_objects.get();
	g_navmesh = m_nav_mesh.get();
}
