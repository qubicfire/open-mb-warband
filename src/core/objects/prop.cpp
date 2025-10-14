#include "core/managers/assets.h"
#include "core/graphics/renderer.h"
#include "core/managers/time.h"

#include "utils/profiler.h"

#include "prop.h"

using namespace mbcore;

void Prop::update()
{
	process_frame();
}

void Prop::load(brf::Mesh* mesh, const Buffer::Types flags)
{
	const auto& frames = mesh->get_frames();

	add_mesh(mesh);
	m_current_frame = 0;
	m_frames = static_cast<int>(
		frames.size()
	);

	if (has_frames())
	{
		mesh->precache(m_aabb, Buffer::Types::Persistent);

		Buffer* vertex_buffer = mesh->get_vertex_array()->get_vertex_buffer();
		m_buffer = vertex_buffer->map_buffer_range<brf::Vertex>();
	}
	else
	{
		mesh->precache(m_aabb, flags);
	}

	add_texture("test/" + mesh->get_material() + ".dds", Texture2D::DDS);
}

void Prop::load(const std::string& name, const Buffer::Types flags)
{
	load(g_assets->get_mesh(name), flags);
}

bool Prop::has_frames() const
{
	return m_frames > 1;
}

void Prop::process_frame()
{
	if (!has_frames() || m_is_frame_stopped)
		return;

	if (Time::get_time() < m_next_time)
		return;

	const auto& frames = get_mesh()->get_frames();

	m_current_frame++;

	if (m_current_frame >= m_frames)
	{
		m_current_frame = 0;
		m_next_frame = 1;
	}
	else
	{
		while (frames[m_current_frame].m_time <= 0.0f)
			m_current_frame++;

		if (m_next_frame >= m_frames - 1)
			m_next_frame = 0;
		else
			m_next_frame = m_current_frame + 1;
	}

	constexpr float INVERSE_TIME = 1.0f / 1000.0f;
	m_next_time = Time::get_time() + frames[m_next_frame].m_time * INVERSE_TIME;

	profiler_start(prop_frame_animation_update);

	//g_threads->detach_task([&]() { set_frame(m_current_frame); });
	set_frame(m_current_frame);
	profiler_stop(prop_frame_animation_update, true);
}

void Prop::set_frame(const int id)
{
	brf::Mesh* mesh = get_mesh();
	const auto& vertices = mesh->get_vertices();
	const auto& faces = mesh->get_faces();
	const auto& frames = mesh->get_frames();
	const brf::Frame& frame = frames[id];
	const brf::Frame& next_frame = frames[m_next_frame];

	for (const auto& face : faces)
	{
		brf::Vertex& v_a = m_buffer[face.i];
		brf::Vertex& v_b = m_buffer[face.j];
		brf::Vertex& v_c = m_buffer[face.k];

		v_a.m_origin = glm::mix(
			frame.m_origins[v_a.m_index],
			next_frame.m_origins[v_a.m_index], 
			glm::vec3(1.0f, 0.0f, 1.0f)
		);

		v_b.m_origin = glm::mix(
			frame.m_origins[v_b.m_index], 
			next_frame.m_origins[v_b.m_index], 
			glm::vec3(1.0f, 0.0f, 1.0f)
		);

		v_c.m_origin = glm::mix(
			frame.m_origins[v_c.m_index],
			next_frame.m_origins[v_c.m_index], 
			glm::vec3(1.0f, 0.0f, 1.0f)
		);
	}
}
