#include "utils/thread_pool.h"
#include "core/graphics/renderer.h"
#include "core/managers/time.h"

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
	static std::mutex mutex {};
	Camera* camera = Renderer::get_camera();
	const glm::mat4& view = camera->get_view();
	const glm::mat4& projection = camera->get_projection();
	const glm::vec3& origin = camera->get_origin();
	Frustum frustum = camera->create_frustum();

	for (const auto& object : m_objects)
	{
		const AABB& aabb = object->get_aabb();
		mb_bit_set<Object::ObjectFlags> flags = object->get_object_flags();

		if (flags.is_bit_set(Object::ObjectFlags::Invisible))
			continue;

		if (frustum.is_visible(aabb))
			continue;

		Shader* shader = object->get_shader();
		shader->bind();
		shader->set_mat4("u_view", view);
		shader->set_mat4("u_projection", projection);
		shader->set_mat4("u_model", object->get_transform());
		shader->set_vec3("u_light_origin", origin);
		shader->set_vec3("u_light_color", glm::vec3(1.0f, 1.0f, 1.0f));

		if (flags.is_bit_set(Object::ObjectFlags::GlobalTime))
			shader->set_float("u_time", Time::get_time());

		if (flags.is_bit_set(Object::ObjectFlags::TextureArrayOld))
		{
			int texture_index = 0;
			for (const auto& texture : object->get_textures())
			{
				// This is uterrly fucking retarted
				// Impossible to use texture 2d array, because
				// some of the textures use different compression. (DXT5 or DXT1). 
				texture->bind(texture_index);

				std::string uniform = "u_textures[" + std::to_string(texture_index) + "]";
				shader->set_int(uniform.c_str(), texture_index);

				texture_index++;
			}
		}
		else
		{
			object->get_texture()->bind();
			shader->set_int("u_texture", 0);
		}

		for (const auto& part : object->get_meshes())
			Renderer::draw_vertex_array(part->m_vertex_array);
	}
}
