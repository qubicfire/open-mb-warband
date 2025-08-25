#include "core/io/file_stream_reader.h"
#include "core/graphics/renderer.h"

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

#ifdef _DEBUG
static void setup_debug_color(const float texture,
	Map::MapVertex& a_v,
	Map::MapVertex& b_v,
	Map::MapVertex& c_v)
{
	if (texture == 0.0)
	{
		a_v.m_color = glm::vec3(0.0, 0.0, 0.8);
		b_v.m_color = glm::vec3(0.0, 0.0, 0.8);
		c_v.m_color = glm::vec3(0.0, 0.0, 0.8);
	}
	else if (texture == 1.0)
	{
		a_v.m_color = glm::vec3(0.75, 0.75, 0.75);
		b_v.m_color = glm::vec3(0.75, 0.75, 0.75);
		c_v.m_color = glm::vec3(0.75, 0.75, 0.75);
	}
	else if (texture == 2.0)
	{
		a_v.m_color = glm::vec3(0.0, 1.0, 0.0);
		b_v.m_color = glm::vec3(0.0, 1.0, 0.0);
		c_v.m_color = glm::vec3(0.0, 1.0, 0.0);
	}
	else if (texture == 3.0)
	{
		a_v.m_color = glm::vec3(0.0, 0.40, 0.0);
		b_v.m_color = glm::vec3(0.0, 0.40, 0.0);
		c_v.m_color = glm::vec3(0.0, 0.40, 0.0);
	}
	else if (texture == 4.0)
	{
		a_v.m_color = glm::vec3(1.0, 1.0, 1.0);
		b_v.m_color = glm::vec3(1.0, 1.0, 1.0);
		c_v.m_color = glm::vec3(1.0, 1.0, 1.0);
	}
	else if (texture == 5.0)
	{
		a_v.m_color = glm::vec3(1.0, 1.0, 0.0);
		b_v.m_color = glm::vec3(1.0, 1.0, 0.0);
		c_v.m_color = glm::vec3(1.0, 1.0, 0.0);
	}
	else if (texture == 6.0)
	{
		a_v.m_color = glm::vec3(0.0, 0.0, 0.0);
		b_v.m_color = glm::vec3(0.0, 0.0, 0.0);
		c_v.m_color = glm::vec3(0.0, 0.0, 0.0);
	}
	else if (texture == 7.0)
	{
		a_v.m_color = glm::vec3(0.0, 0.0, 0.5);
		b_v.m_color = glm::vec3(0.0, 0.0, 0.5);
		c_v.m_color = glm::vec3(0.0, 0.0, 0.5);
	}
	else
	{
		a_v.m_color = glm::vec3(0.0, 0.0, 0.0);
		b_v.m_color = glm::vec3(0.0, 0.0, 0.0);
		c_v.m_color = glm::vec3(0.0, 0.0, 0.0);
	}
}
#endif // _DEBUG

void Map::client_start()
{
	FileStreamReader stream {};
	stream.open("test/map.txt");

	uint32_t vertices_count = stream.number_from_chars<uint32_t>();
	m_vertices.resize(vertices_count);

	float max_vertex_x = 0.0f;
	float max_vertex_y = 0.0f;

	std::vector<std::vector<MapVertex>> parts;
	parts.resize(8);

	for (auto& vertex : m_vertices)
	{
		// Can't read just glm::vec3 because map.txt contents only TEXT not BYTES
		// We have to read float one by one
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

	uint32_t indices_count = stream.number_from_chars<uint32_t>() * 3;
	m_indices.resize(indices_count);

	for (uint32_t i = 0; i < indices_count; i += 3)
	{
		float texture = stream.number_from_chars<float>();
		stream.read<std::string_view>(); // unused
		stream.read<std::string_view>(); // unused

		int a = stream.number_from_chars<int>();
		int b = stream.number_from_chars<int>();
		int c = stream.number_from_chars<int>();

		m_indices[i] = a;
		m_indices[i + 1] = b;
		m_indices[i + 2] = c;	

		MapVertex& a_v = m_vertices[a];
		MapVertex& b_v = m_vertices[b];
		MapVertex& c_v = m_vertices[c];

		if (texture > 5.0f)
			texture = texture - 1.0f;

		if (texture > 7.0f)
			texture = texture - 7.0f;

		//if (texture > a_v.m_type)
		//	a_v.m_type = texture;
		//if (texture > b_v.m_type)
		//	b_v.m_type = texture;
		//if (texture > c_v.m_type)
		//	c_v.m_type = texture;

		a_v.m_type = texture;
		b_v.m_type = texture;
		c_v.m_type = texture;

#ifdef _DEBUG
		setup_debug_color(texture, a_v, b_v, c_v);
#endif // _DEBUG

		a_v.m_texture = glm::vec2(a_v.m_origin.x / max_vertex_x, a_v.m_origin.z / max_vertex_y);
		b_v.m_texture = glm::vec2(b_v.m_origin.x / max_vertex_x, b_v.m_origin.z / max_vertex_y);
		c_v.m_texture = glm::vec2(c_v.m_origin.x / max_vertex_x, c_v.m_origin.z / max_vertex_y);

		auto& part = parts[static_cast<int>(texture)];

		part.push_back(a_v);
		part.push_back(b_v);
		part.push_back(c_v);
		
		// TODO: Calculate normal
	}

	for (auto& part : parts)
	{
		part.shrink_to_fit();

		Unique<VertexArray> vertex_array = VertexArray::create();
		Unique<VertexBuffer> vertex_buffer = VertexBuffer::create(part);

		vertex_array->link(0, VertexType::Float3, cast_offset(MapVertex, m_origin));
		vertex_array->link(1, VertexType::Float2, cast_offset(MapVertex, m_texture));
		vertex_array->link(2, VertexType::Float, cast_offset(MapVertex, m_type));
	#ifdef _DEBUG
		vertex_array->link(3, VertexType::Float3, cast_offset(MapVertex, m_color));
	#endif // _DEBUG

		// No need to use index buffer
		vertex_array->set_vertex_buffer(vertex_buffer);

		vertex_array->unbind();
		m_arrays.emplace_back(std::move(vertex_array));
	}

	add_texture("test/ocean.dds", Texture2D::DDS);
	add_texture("test/mountain.dds", Texture2D::DDS);
	add_texture("test/map_steppe.dds", Texture2D::DDS);
	add_texture("test/plain.dds", Texture2D::DDS);
	add_texture("test/snow.dds", Texture2D::DDS);
	add_texture("test/desert.dds", Texture2D::DDS);

	add_texture("test/ocean.dds", Texture2D::DDS);
	add_texture("test/river.dds", Texture2D::DDS);
}

void Map::draw()
{
#ifdef _DEBUG
	static Shader* shader = nullptr;
	if (m_is_debug_enable)
		shader = g_assets->get_shader("map_terrain_debug");
	else
		shader = g_assets->get_shader("map_terrain");
#endif // _DEBUG

	for (const auto& array : m_arrays)
	{
		Renderer::prepare_model_projection(shader,
			m_origin,
			m_rotation, 
			m_scale, 
			m_angle);

		bind_all_textures(shader);

		Renderer::draw_triangles(array);
	}
}

void Map::bind_all_textures(Shader* shader) const
{
	int texture_index = 0;

	shader->set_float("u_time", Time::get_time());

	for (const auto& texture : m_textures)
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

void Map::add_texture(const std::string& path, const Texture2D::Type type)
{
	Texture2D* texture = Texture2D::create(path, type);
	m_textures.emplace_back(texture);
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

		constexpr float CENTER_DIVISON = 1.0f / 3.0f;

		float center_x = (v_a.x + v_b.x + v_c.x) * CENTER_DIVISON;
		float center_z = (v_a.z + v_b.z + v_c.z) * CENTER_DIVISON;

		float compute_x = std::abs(center_x - x);
		float compute_z = std::abs(center_z - y);

		if (compute_x < closest_x && compute_z < closest_z)
		{
			closest_x = compute_x;
			closest_z = compute_z;

			result_coord = (v_a.y + v_b.y + v_c.y) * CENTER_DIVISON;
		}
	}

	return glm::vec3(x, result_coord, -y);
}
