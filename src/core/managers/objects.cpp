#include "objects.h"

void ObjectManager::remove_object(Object* object)
{
	if (object == nullptr)
		return;

	auto& last = m_objects.back();
	last->m_id = object->m_id;

	m_objects[last->m_id].reset(last.get());
	m_objects.pop_back();
}

void ObjectManager::remove_all()
{
	for (auto& object : m_objects)
		object.reset();

	m_objects.clear();
}

void ObjectManager::draw_all()
{
	for (const auto& object : m_objects)
		object->draw();
}
