#include "objects.h"

#include "core/graphics/renderer.h"
#include "core/managers/time.h"
#include "core/managers/assets.h"

#include "utils/profiler.h"

void ObjectManager::initialize()
{
	m_culling_shader = g_assets->load_shader(
		"_cs_culling_default",
		"test/cs_main.glsl",
		Shader::Compute
	);
}

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

void ObjectManager::update_all()
{
	for (const auto& object : m_objects)
		object->update();
}

void ObjectManager::draw_all()
{
	Camera* camera = Renderer::get_camera();
	const glm::mat4& view = camera->get_view();
	const glm::mat4& projection = camera->get_projection();
	const glm::vec3& origin = camera->get_origin();
	Frustum frustum = camera->create_frustum();

	for (const auto& object : m_objects)
	{
		auto flags = object->get_object_flags();
		if (flags.is_bit_set(Object::Flags::Invisible))
			continue;

		//const AABB& aabb = object->get_world_aabb();
		//if (!frustum.is_visible(aabb))
		//	continue;

		Shader* shader = object->get_shader();
		shader->bind();
		shader->set_mat4("u_view", view);
		shader->set_mat4("u_projection", projection);
		shader->set_mat4("u_model", object->get_transform());
		shader->set_vec3("u_light_origin", origin);
		shader->set_vec3("u_light_color", glm::vec3(1.0f, 1.0f, 1.0f));

		if (flags.is_bit_set(Object::Flags::GlobalTime))
			shader->set_float("u_time", Time::get_time());

		if (flags.is_bit_set(Object::Flags::Billboard))
			shader->set_mat4("u_projection_view", camera->get_inverse_projection());

		if (flags.is_bit_set(Object::Flags::TextureArray))
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

		if (flags.is_bit_set(Object::Flags::FrameSystem))
		{
			profiler_start(frame_animation_update);

			//shader->set_bool("u_use_frame_system", true);

			object->get_mesh()->update_frame_vertices();

			profiler_stop(frame_animation_update, true);
		}
		//else
		//{
		//	shader->set_bool("u_use_frame_system", false);
		//}

#ifdef _DEBUG
		//if (m_is_aabb_enabled)
		//	draw_aabb(object);
#endif // _DEBUG

		for (const auto& part : object->get_meshes())
			Renderer::draw_vertex_array(part->get_vertex_array());
	}
}

#ifdef _DEBUG
#include <GLFW/glfw3.h>

void ObjectManager::draw_aabb(const mb_unique<Object>& object)
{
	Camera* camera = Renderer::get_camera();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(
		glm::value_ptr(camera->get_view())
	);
	glMultMatrixf(
		glm::value_ptr(object->get_transform())
	);

	glLineWidth(2.0f);
	glBegin(GL_TRIANGLES);
	{
		const auto& aabb = object->get_aabb();
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		// body
		glVertex3f(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z);
		glVertex3f(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z);
		glVertex3f(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z);

		glVertex3f(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z);
		glVertex3f(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z);
		glVertex3f(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z);

		glVertex3f(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z);
		glVertex3f(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z);
		glVertex3f(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z);

		glVertex3f(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z);
		glVertex3f(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z);
		glVertex3f(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z);

		glVertex3f(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z);
		glVertex3f(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z);
		glVertex3f(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z);

		glVertex3f(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z);
		glVertex3f(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z);
		glVertex3f(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z);

		glVertex3f(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z);
		glVertex3f(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z);
		glVertex3f(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z);

		glVertex3f(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z);
		glVertex3f(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z);
		glVertex3f(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z);

		// top
		glVertex3f(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z);
		glVertex3f(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z);
		glVertex3f(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z);

		glVertex3f(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z);
		glVertex3f(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z);
		glVertex3f(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z);

		// bottom
		glVertex3f(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z);
		glVertex3f(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z);
		glVertex3f(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z);

		glVertex3f(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z);
		glVertex3f(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z);
		glVertex3f(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z);
	}
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
}

#endif // _DEBUG
