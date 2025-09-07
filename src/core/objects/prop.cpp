#include "core/managers/assets.h"
#include "core/graphics/renderer.h"
#include "core/managers/time.h"

#include "prop.h"

using namespace mbcore;

void Prop::update()
{
	process_frame();
}

void Prop::load(brf::Mesh* mesh, int flags)
{
	const auto& frames = mesh->get_frames();

	m_mesh = mesh;
	m_current_frame = 0;
	m_frames = static_cast<int>(
		frames.size()
	);

	if (has_frames())
	{
		m_mesh->precache(BufferFlags::Persistent);

		VertexBuffer* vertex_buffer = m_mesh->m_vertex_array->get_vertex_buffer();
		m_buffer = vertex_buffer->map_buffer_range<brf::Vertex>();
	}
	else
	{
		m_mesh->precache(flags);
	}

	std::string texture_path = "test/" + m_mesh->get_material() + ".dds";
	m_texture = Texture2D::create(texture_path, Texture2D::DDS);
}

void Prop::load(const std::string& name, int flags)
{
	load(g_assets->get_mesh(name), flags);
}

brf::Mesh* Prop::get_mesh() const
{
	return m_mesh;
}

bool Prop::has_frames() const
{
	return m_frames > 1;
}

void Prop::draw_internal(Shader* shader)
{
	Renderer::build_model_projection(shader, m_origin, m_rotation, m_scale, m_angle);

	bind_all_textures(shader);

	Renderer::draw_vertex_array(m_mesh->m_vertex_array);
}

void Prop::bind_all_textures(Shader* shader) const
{
	m_texture->bind();
	shader->set_int("u_texture", 0);
}

void Prop::process_frame()
{
	if (!has_frames() || m_is_frame_stopped)
		return;

	if (Time::get_time() < m_next_time)
		return;

	const auto& frames = m_mesh->get_frames();

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

	set_frame(m_current_frame);
}

void Prop::set_frame(const int id)
{
	const auto& vertices = m_mesh->get_vertices();
	const auto& indices = m_mesh->get_indices();
	const auto& frames = m_mesh->get_frames();
	const brf::Frame& frame = frames[id];
	const brf::Frame& next_frame = frames[m_next_frame];

	// Persistent Mapped Buffer + Unsynchronized Access
	for (uint32_t i = 0; i < indices.size(); i += 3)
	{
		brf::Vertex& v_a = m_buffer[indices[i]];
		brf::Vertex& v_b = m_buffer[indices[i + 1]];
		brf::Vertex& v_c = m_buffer[indices[i + 2]];

		//v_a.m_origin = frame.m_origins[v_a.m_index];
		//v_b.m_origin = frame.m_origins[v_b.m_index];
		//v_c.m_origin = frame.m_origins[v_c.m_index];

		v_a.m_origin = glm::mix(frame.m_origins[v_a.m_index], next_frame.m_origins[v_a.m_index], glm::vec3(1.0f, 0.0f, 1.0f));
		v_b.m_origin = glm::mix(frame.m_origins[v_b.m_index], next_frame.m_origins[v_b.m_index], glm::vec3(1.0f, 0.0f, 1.0f));
		v_c.m_origin = glm::mix(frame.m_origins[v_c.m_index], next_frame.m_origins[v_c.m_index], glm::vec3(1.0f, 0.0f, 1.0f));
	}
}
