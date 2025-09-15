#include "core/managers/objects.h"
#include "core/managers/assets.h"

#include "scene.h"

void Scene::setup()
{
	if (!m_objects)
		m_objects = create_unique<ObjectManager>();

	g_objects = m_objects.get();

	//for (const auto& [name, shader] : g_assets->m_shaders)
	//	g_objects->add_object_group(name, shader.get());
}
