#include "core/graphics/renderer.h"
#include "core/managers/objects.h"

#include "game/map_icons_loader.h"

#include "map_scene.h"

#include "game/objects/map.h"

void MapScene::start()
{
	m_camera = Object::instantiate<Camera>();
 	m_test = Object::instantiate<Test>();
 	Test* test = Object::instantiate<Test>();
	Map* map = Object::instantiate<Map>();

	test->set_animation_frame(5);

	MapIconsLoader icons_loader;
	icons_loader.load();

	PartiesLoader parties_loader;
	parties_loader.load(map, icons_loader);

	Renderer::setup_camera_object(m_camera);
}

void MapScene::update_client()
{
	m_objects->update_all();
}

void MapScene::update()
{
	m_test->server_send_packet();
}
