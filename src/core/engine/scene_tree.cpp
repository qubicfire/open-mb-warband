#include "scene_tree.h"

void SceneTree::pop()
{
	m_scene->dispose();

	m_scenes.pop();

	if (m_scenes.empty())
		return;

	m_scene = m_scenes.top().get();
	m_scene->replace_globals();
}

void SceneTree::update_client()
{
	m_scene->update_client();
}

void SceneTree::update()
{
	m_scene->update();
}
