#include "core/graphics/renderer.h"
#include "core/managers/objects.h"

#include "game/map_icons_loader.h"

#include "map_scene.h"

#include "game/objects/map.h"

void MapScene::start()
{
	m_camera = GameObject::instantiate<Camera>();
	m_test = GameObject::instantiate<Test>();
	Map* map = GameObject::instantiate<Map>();

	MapIconsLoader icons_loader;
	icons_loader.load();

	PartiesLoader parties_loader;
	parties_loader.load(map, icons_loader);

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
