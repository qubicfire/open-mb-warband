#ifndef _SCENE_TREE_H
#define _SCENE_TREE_H
#include <stack>

#include "core/mb_type_traits.h"
#include "scene.h"

class SceneTree
{
public:
	template <class _Tx>
	inline void push()
	{
		mb_unique<_Tx> unique_scene = create_unique<_Tx>();
		m_scene = unique_scene.get();

		m_scenes.push(std::move(unique_scene));

		m_scene->setup();
		m_scene->start();
	}

	inline void pop()
	{
		m_scene->dispose();

		m_scenes.pop();

		m_scene = m_scenes.top().get();
		m_scene->setup();
	}

	inline void client_update()
	{
		m_scene->client_update();
	}

	inline void update()
	{
		m_scene->update();
	}
private:
	std::stack<mb_unique<Scene>> m_scenes;

	Scene* m_scene;
};

#endif // !_SCENE_TREE_H
