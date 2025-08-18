#ifndef _MAP_SCENE_H
#define _MAP_SCENE_H
#include "core/engine/scene.h"
#include "core/objects/camera.h"

#include "game/parties_loader.h"

#include "game/objects/party.h"
#include "game/objects/test.h"

class MapScene : public Scene
{
public:
	void start() override;

	void client_update() override;
	void update() override;
private:
	Camera* m_camera;
	Test* m_test;
};

#endif // !_MAP_SCENE_H
