#ifndef _SCENE_TREE_H
#define _SCENE_TREE_H
#include "scene.h"

#include <stack>

class SceneTree
{
public:
	template <class _Tx>
	inline void push()
	{
		mb_unique<_Tx> scene = create_unique<_Tx>();

		m_scene = scene.get();

		m_scenes.push(std::move(scene));

		m_scene->setup();
		m_scene->start();
	}

	void pop();

	void update_client();
	void update();
private:
	std::stack<mb_unique<Scene>> m_scenes;

	Scene* m_scene;
};

#endif // !_SCENE_TREE_H
