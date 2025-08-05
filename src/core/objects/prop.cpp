#include "core/managers/assets.h"
#include "core/graphics/renderer.h"

#include "prop.h"

using namespace mbcore;

void Prop::load(brf::Mesh* mesh, bool is_static_draw)
{
	m_mesh = mesh;

	const auto& vertices = m_mesh->get_vertices();
	const auto& indices = m_mesh->get_indices();

	m_vertex_array = VertexArray::create();
	Unique<VertexBuffer> vertex_buffer = VertexBuffer::create(vertices, is_static_draw);

	m_vertex_array->link(0, VertexType::Float3, cast_offset(brf::Vertex, m_origin));
	m_vertex_array->link(1, VertexType::Float3, cast_offset(brf::Vertex, m_normal));
	m_vertex_array->link(2, VertexType::Float2, cast_offset(brf::Vertex, m_texture_a));
	m_vertex_array->link(3, VertexType::Uint, cast_offset(brf::Vertex, m_color));

	Unique<IndexBuffer> index_buffer = IndexBuffer::create(indices);

	m_vertex_array->set_vertex_buffer(vertex_buffer);
	m_vertex_array->set_index_buffer(index_buffer);

	//std::string texture_path = "test/" + m_mesh->get_material() + ".dds";
	std::string texture_path = "test/fake_houses.dds";
	m_texture = Texture2D::create(texture_path, Texture2D::DDS);

	m_vertex_array->unbind();
}

void Prop::load(const std::string& name, bool is_static_draw)
{
	load(g_assets->get_mesh(name), is_static_draw);
}

brf::Mesh* Prop::get_mesh() const noexcept
{
	return m_mesh;
}

void Prop::bind_all_textures(Shader* shader) const
{
	m_texture->bind();
	shader->set_int("u_texture", 0);
}
