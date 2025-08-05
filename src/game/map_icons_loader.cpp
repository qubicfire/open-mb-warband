#include "core/managers/assets.h"

#include "map_icons_loader.h"

using namespace mbcore;

MapIcon build_icon_render_info(brf::Mesh* mesh)
{
	MapIcon icon {};

	if (mesh == nullptr)
		return icon;

	const auto& vertices = mesh->get_vertices();
	const auto& indices = mesh->get_indices();

	icon.m_array = VertexArray::create();
	Unique<VertexBuffer> vertex_buffer = VertexBuffer::create(vertices, true);

	icon.m_array->link(0, VertexType::Float3, cast_offset(brf::Vertex, m_origin));
	icon.m_array->link(1, VertexType::Float3, cast_offset(brf::Vertex, m_normal));
	icon.m_array->link(2, VertexType::Float2, cast_offset(brf::Vertex, m_texture_a));
	icon.m_array->link(3, VertexType::Uint, cast_offset(brf::Vertex, m_color));

	Unique<IndexBuffer> index_buffer = IndexBuffer::create(indices);

	icon.m_array->set_vertex_buffer(vertex_buffer);
	icon.m_array->set_index_buffer(index_buffer);

	std::string texture_path = "test/" + mesh->get_material() + ".dds";
	icon.m_texture = Texture2D::create(texture_path, Texture2D::DDS);

	icon.m_array->unbind();
	return icon;
}

bool MapIconsLoader::load(ThreadPool& pool)
{
	FileStreamReader stream {};
	if (!stream.open("test/map_icons.txt"))
		return false;

	int icons_count = load_descriptor(stream);
	if (icons_count <= 0)
		return false;

	// https://earendil_ardamire.gitlab.io/modding-guide/Subpages/Documentation_Module_System/Module_Map_Icons.html
	for (int i = 0; i < icons_count; i++)
	{
		std::string icon_id = "icon_" + stream.read_until(' ', '\n');
		int flags = stream.number_from_chars<int>();
		std::string name = stream.read_until(' ', '\n');
		float scale = stream.number_from_chars<float>();
		int sound_id = stream.number_from_chars<int>();

		float offset_y = stream.number_from_chars<float>();
		float offset_x = stream.number_from_chars<float>();
		float offset_z = stream.number_from_chars<float>();

		brf::Mesh* mesh = g_assets->get_mesh(name);
		m_icons.push_back(build_icon_render_info(mesh));
	}

	return true;
}

int MapIconsLoader::load_descriptor(FileStreamReader& stream) const
{
	const auto file_id = stream.read<std::string_view>();
	const auto version_word = stream.read<std::string_view>();
	const auto version = stream.read<std::string_view>();

	if (file_id != "map_icons_file" || version_word != "version" || version != "1")
		return 0;

	return stream.number_from_chars<int>();
}

MapIcon* MapIconsLoader::get_icon(int index)
{
	return &m_icons[index];
}
