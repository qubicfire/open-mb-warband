#include "core/graphics/renderer.h"


#include "core/managers/objects.h"

#include "map_scene.h"

#include "core/objects/map.h"

void MapScene::start()
{
	m_camera = GameObject::instantiate<Camera>();
	Map* map = GameObject::instantiate<Map>();

	m_icons_loader.load();

	PartiesLoader parties_loader;
	parties_loader.load(map, m_icons_loader);

	Renderer::setup_camera_object(m_camera);
}

void MapScene::update()
{
	m_camera->update();
}
