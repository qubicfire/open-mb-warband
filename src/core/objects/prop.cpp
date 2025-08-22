#include "core/managers/assets.h"
#include "core/graphics/renderer.h"

#include "prop.h"

using namespace mbcore;

void Prop::load(brf::Mesh* mesh, int flags)
{
	const auto& frames = m_mesh->get_frames();

	m_mesh = mesh;
	m_frame_id = 0;
	m_all_frames = static_cast<int>(
		frames.size()
	);

	m_mesh->precache(flags | BufferFlags::Persistent);
	m_buffer = m_mesh->m_vertex_array->get_vertex_buffer()->map_buffer_range<brf::Vertex>();

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
	return m_all_frames > 1;
}

void Prop::draw_internal(Shader* shader)
{
	if (has_frames())
		process_frame(m_buffer, m_frame_id);

	Renderer::prepare_model_projection(shader, m_origin, m_rotation, m_scale, m_angle);

	bind_all_textures(shader);

	Renderer::draw_indexed(m_mesh->m_vertex_array);
}

void Prop::bind_all_textures(Shader* shader) const
{
	m_texture->bind();
	shader->set_int("u_texture", 0);
}

void Prop::process_frame(brf::Vertex* buffer, int frame_id)
{
	// TODO: Persistent Mapped Buffer + Unsynchronized Access
	if (m_frame_id + 1 >= m_all_frames)
		m_frame_id = 0;
	else
		m_frame_id++;

	const auto& vertices = m_mesh->get_vertices();
	const auto& indices = m_mesh->get_indices();
	const auto& frames = m_mesh->get_frames();
	const brf::Frame& frame = frames[frame_id];

	for (uint32_t i = 0; i < indices.size(); i += 3)
	{
		brf::Vertex& v_a = buffer[indices[i]];
		brf::Vertex& v_b = buffer[indices[i + 1]];
		brf::Vertex& v_c = buffer[indices[i + 2]];

		v_a.m_origin = frame.m_origins[v_a.m_index];
		v_b.m_origin = frame.m_origins[v_b.m_index];
		v_c.m_origin = frame.m_origins[v_c.m_index];
	}
}
