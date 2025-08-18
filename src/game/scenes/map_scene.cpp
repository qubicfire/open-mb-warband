#include "core/graphics/renderer.h"
#include "core/managers/objects.h"

#include "map_scene.h"

#include "game/objects/map.h"

void MapScene::start()
{
	m_camera = GameObject::instantiate<Camera>();
	m_test = GameObject::instantiate<Test>();
	Map* map = GameObject::instantiate<Map>();

	m_icons_loader.load();

	PartiesLoader parties_loader;
	parties_loader.load(map, m_icons_loader);

	Renderer::setup_camera_object(m_camera);
}

void MapScene::client_update()
{
	m_camera->update();
}

void MapScene::update()
{
	m_test->server_send_packet();
}
