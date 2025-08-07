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
		Unique<_Tx> unique_scene = create_unique<_Tx>();
		m_current = unique_scene.get();

		m_scenes.push(std::move(unique_scene));

		m_current->start();
	}

	inline void pop()
	{
		m_current->dispose();

		m_scenes.pop();

		m_current = m_scenes.top().get();
	}

	inline void update()
	{
		m_current->update();
	}
private:
	std::stack<Unique<Scene>> m_scenes;
	Scene* m_current;
};

#endif // !_SCENE_TREE_H
