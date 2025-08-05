#include "core/io/file_stream_reader.h"

#include "core/managers/time.h"
#include "core/managers/assets.h"

#include "map.h"

using namespace mbcore;

enum class TerrainCodes : uint8_t
{
	Water,
	Mountain,
	Steppe,
	Plain,
	Snow,
	Desert = 5,
	Bridge = 7,
	River,
	MountainForest,
	SteppeForest,
	Forest,
	SnowForest,
	DesertForest,
	Count
};

void Map::start()
{
	FileStreamReader stream {};
	stream.open("test/map.txt");

	auto vertices_count = stream.number_from_chars<uint32_t>();
	m_vertices.resize(vertices_count);

	float max_vertex_x = 0.0f;
	float max_vertex_y = 0.0f;

	for (auto& vertex : m_vertices)
	{
		// Can't read just glm::vec3 because map.txt contents only TEXT not BYTES
		// We have to read float by float
		vertex.m_origin.x = stream.number_from_chars<float>();
		vertex.m_origin.y = stream.number_from_chars<float>();
		vertex.m_origin.z = stream.number_from_chars<float>();

		// Little-endian?
		float temp = vertex.m_origin.y;
		vertex.m_origin.y = vertex.m_origin.z;
		vertex.m_origin.z = -temp;

		if (vertex.m_origin.x > max_vertex_x)
			max_vertex_x = vertex.m_origin.x;
		if (vertex.m_origin.z > max_vertex_y)
			max_vertex_y = vertex.m_origin.z;
	}

	auto indices_count = stream.number_from_chars<uint32_t>() * 3;
	m_indices.resize(indices_count);
	float previous_terrain {};

	for (uint32_t i = 0; i < indices_count; i += 3)
	{
		auto terrain_type = stream.number_from_chars<float>();
		stream.read<std::string_view>(); // unused
		stream.read<std::string_view>(); // unused

		auto a = stream.number_from_chars<int>();
		auto b = stream.number_from_chars<int>();
		auto c = stream.number_from_chars<int>();

		m_indices[i] = a;
		m_indices[i + 1] = b;
		m_indices[i + 2] = c;	

		MapVertex& a_v = m_vertices[a];
		MapVertex& b_v = m_vertices[b];
		MapVertex& c_v = m_vertices[c];

		if (terrain_type == 7.0f)
		{
			a_v.m_type = previous_terrain;
			b_v.m_type = previous_terrain;
			c_v.m_type = previous_terrain;
		}
		else
		{
			if (terrain_type > 8.0f)
				terrain_type = terrain_type - 8.0f;

			if (terrain_type > a_v.m_type)
				a_v.m_type = terrain_type;
			if (terrain_type > b_v.m_type)
				b_v.m_type = terrain_type;
			if (terrain_type > c_v.m_type)
				c_v.m_type = terrain_type;
		}

		if (terrain_type != 8.0f)
			previous_terrain = terrain_type;

		a_v.m_texture = glm::vec2(a_v.m_origin.x / max_vertex_x, a_v.m_origin.z / max_vertex_y);
		b_v.m_texture = glm::vec2(b_v.m_origin.x / max_vertex_x, b_v.m_origin.z / max_vertex_y);
		c_v.m_texture = glm::vec2(c_v.m_origin.x / max_vertex_x, c_v.m_origin.z / max_vertex_y);

		// TODO: Calculate normal
	}

	m_vertex_array = VertexArray::create();
	Unique<VertexBuffer> vertex_buffer = VertexBuffer::create(m_vertices);

	m_vertex_array->link(0, VertexType::Float3, cast_offset(MapVertex, m_origin));
	m_vertex_array->link(1, VertexType::Float2, cast_offset(MapVertex, m_texture));
	m_vertex_array->link(2, VertexType::Float, cast_offset(MapVertex, m_type));

	Unique<IndexBuffer> index_buffer = IndexBuffer::create(m_indices);

	m_vertex_array->set_vertex_buffer(vertex_buffer);
	m_vertex_array->set_index_buffer(index_buffer);

	add_texture("test/ocean.dds", Texture2D::DDS);
	add_texture("test/mountain.dds", Texture2D::DDS);
	add_texture("test/map_steppe.dds", Texture2D::DDS);
	add_texture("test/plain.dds", Texture2D::DDS);
	add_texture("test/snow.dds", Texture2D::DDS);
	add_texture("test/desert.dds", Texture2D::DDS);

	//add_texture("test/river.dds", Texture2D::DDS);

	add_texture("test/map_steppe.dds", Texture2D::DDS); // BRIDGE
	add_texture("test/river.dds", Texture2D::DDS); //RIVER
	//add_texture("test/mountain.dds", Texture2D::DDS);
	//add_texture("test/map_steppe.dds", Texture2D::DDS);
	//add_texture("test/plain.dds", Texture2D::DDS);
	//add_texture("test/snow.dds", Texture2D::DDS);
	//add_texture("test/desert.dds", Texture2D::DDS);

	m_vertex_array->unbind();
}

void Map::draw()
{
	static Shader* shader = g_assets->get_shader("map_terrain");

	BaseClass::draw_internal(shader);
}

void Map::bind_all_textures(Shader* shader) const
{
	int texture_index {};

	shader->set_float("u_time", Time::get_time());

	for (const auto& texture : m_textures)
	{
		texture->bind(texture_index);

		std::string uniform = "u_textures[" + std::to_string(texture_index) + "]";
		shader->set_int(uniform.c_str(), texture_index);

		texture_index++;
	}
}

void Map::add_texture(const std::string& path, const Texture2D::Type type)
{
	Unique<Texture2D> texture = Texture2D::create(path, type);
	m_textures.emplace_back(std::move(texture));
}

glm::vec3 Map::align_point_to_ground(float x, float y)
{
	float closest_x = std::numeric_limits<float>::max();
	float closest_z = std::numeric_limits<float>::max();
	float result_coord = 0.0f;

	for (uint32_t i = 0; i < m_indices.size(); i += 3)
	{
		glm::vec3& v_a = m_vertices[m_indices[i]].m_origin;
		glm::vec3& v_b = m_vertices[m_indices[i + 1]].m_origin;
		glm::vec3& v_c = m_vertices[m_indices[i + 2]].m_origin;

		float center_x = (v_a.x + v_b.x + v_c.x) / 3.0f;
		float center_z = (v_a.z + v_b.z + v_c.z) / 3.0f;

		float compute_x = std::abs(center_x - x);
		float compute_z = std::abs(center_z - y);

		if (compute_x < closest_x && compute_z < closest_z)
		{
			closest_x = compute_x;
			closest_z = compute_z;

			result_coord = (v_a.y + v_b.y + v_c.y) / 3.0f;
		}
	}

	return glm::vec3(x, result_coord, -y);
}
