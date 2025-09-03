#include "core/io/file_stream_reader.h"
#include "core/graphics/renderer.h"

#include "core/managers/time.h"
#include "core/managers/assets.h"

#include "map.h"

using namespace mbcore;

enum TerrainCodes : uint8_t
{
	rt_water,
	rt_mountain,
	rt_steppe,
	rt_plain,
	rt_snow,
	rt_desert = 5,
	// rt_gap_for_no_fucking_reason = 6,
	rt_bridge = 7,
	rt_river,
	// rt_mountain_forest,
	// rt_steppe_forest,
	// rt_forest,
	// rt_snow_forest,
	// rt_desert_forest,
	rt_count = rt_river
};

#ifdef _DEBUG
static void setup_debug_color(const float texture,
	Map::MapVertex& a_v,
	Map::MapVertex& b_v,
	Map::MapVertex& c_v)
{
	static const std::array<glm::vec3, 8> colors = 
	{
		glm::vec3(0.0, 0.0, 0.8),
		glm::vec3(0.75, 0.75, 0.75),
		glm::vec3(0.0, 1.0, 0.0),
		glm::vec3(0.0, 0.40, 0.0),
		glm::vec3(1.0, 1.0, 1.0),
		glm::vec3(1.0, 1.0, 0.0),
		glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(0.0, 0.0, 0.5),
	};

	const glm::vec3 color = colors[static_cast<int>(texture)];
	a_v.m_color = color;
	b_v.m_color = color;
	c_v.m_color = color;
}
#endif // _DEBUG

#include "core/managers/physics.h"

void Map::start_client()
{
	float max_vertex_x = std::numeric_limits<float>::min();
	float max_vertex_y = std::numeric_limits<float>::min();

	FileStreamReader stream {};
	stream.open("test/map.txt");

	uint32_t vertices_count = stream.number_from_chars<uint32_t>();
	m_vertices.resize(vertices_count);

	m_arrays.reserve(TerrainCodes::rt_count);
	m_textures.reserve(TerrainCodes::rt_count);

	for (auto& vertex : m_vertices)
	{
		// Can't read just glm::vec3 because map.txt contents only TEXT not BYTES
		// We have to read float one by one
		vertex.x = stream.number_from_chars<float>();
		vertex.y = stream.number_from_chars<float>();
		vertex.z = stream.number_from_chars<float>();

		// Little-endian?
		float temp = vertex.y;
		vertex.y = vertex.z;
		vertex.z = -temp;

		if (vertex.x > max_vertex_x)
			max_vertex_x = vertex.x;
		if (vertex.z > max_vertex_y)
			max_vertex_y = vertex.z;
	}

	uint32_t indices_count = stream.number_from_chars<uint32_t>() * 3;
	m_indices.resize(indices_count);

	std::vector<Map::MapVertex> vertices {};
	vertices.reserve(indices_count);

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

		MapVertex map_a_v {};
		MapVertex map_b_v {};
		MapVertex map_c_v {};

		map_a_v.m_origin = m_vertices[a];
		map_b_v.m_origin = m_vertices[b];
		map_c_v.m_origin = m_vertices[c];

		if (texture > 5.0f)
			texture = texture - 1.0f;

		if (texture > 7.0f)
			texture = texture - 7.0f;

		map_a_v.m_type = texture;
		map_b_v.m_type = texture;
		map_c_v.m_type = texture;

#ifdef _DEBUG
		setup_debug_color(texture, map_a_v, map_b_v, map_c_v);
#endif // _DEBUG

		map_a_v.m_texture = glm::vec2(map_a_v.m_origin.x / max_vertex_x, map_a_v.m_origin.z / max_vertex_y);
		map_b_v.m_texture = glm::vec2(map_b_v.m_origin.x / max_vertex_x, map_b_v.m_origin.z / max_vertex_y);
		map_c_v.m_texture = glm::vec2(map_c_v.m_origin.x / max_vertex_x, map_c_v.m_origin.z / max_vertex_y);

		vertices.push_back(map_a_v);
		vertices.push_back(map_b_v);
		vertices.push_back(map_c_v);
		// TODO: Calculate normal
	}

	m_vertex_array = VertexArray::create();
	Unique<VertexBuffer> vertex_buffer = VertexBuffer::create(vertices);

	m_vertex_array->link(0, VertexType::Float3, cast_offset(MapVertex, m_origin));
	m_vertex_array->link(1, VertexType::Float2, cast_offset(MapVertex, m_texture));
	m_vertex_array->link(2, VertexType::Float, cast_offset(MapVertex, m_type));
#ifdef _DEBUG
	m_vertex_array->link(3, VertexType::Float3, cast_offset(MapVertex, m_color));
#endif // _DEBUG

	// Doesn't have to use index buffer
	m_vertex_array->set_vertex_buffer(vertex_buffer);

	m_vertex_array->unbind();

	add_texture("test/ocean.dds", Texture2D::DDS);
	add_texture("test/mountain.dds", Texture2D::DDS);
	add_texture("test/map_steppe.dds", Texture2D::DDS);
	add_texture("test/plain.dds", Texture2D::DDS);
	add_texture("test/snow.dds", Texture2D::DDS);
	add_texture("test/desert.dds", Texture2D::DDS);

	add_texture("test/ocean.dds", Texture2D::DDS); // bridge texture ??????
	add_texture("test/ocean.dds", Texture2D::DDS); // should be river, but meh

	m_body.create_body(this,
		m_vertices,
		m_indices,
		MotionType::Static,
		ActivationType::Activate,
		0);
}

void Map::draw()
{
#ifdef _DEBUG
	static Shader* shader = nullptr;
	if (m_is_debug_enable)
		shader = g_assets->get_shader("map_terrain_debug");
	else
		shader = g_assets->get_shader("map_terrain");
#else
	static Shader* shader = g_assets->get_shader("map_terrain");
#endif // _DEBUG

	Renderer::build_model_projection(shader,
		m_origin,
		m_rotation, 
		m_scale, 
		m_angle);

	bind_all_textures(shader);

	Renderer::draw_triangles(m_vertex_array);
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
	// We went from finding the nearest model vertex
	// to casting an actual ray to the model. What a magic out there
	RayCastInfo info {};
	Ray ray(glm::vec3(x, 30.0f, -y), glm::vec3(0.0f, -1.0f, 0.0f), 100.0f);

	if (Physics::raycast(ray, info))
	{
		log_print("Raycast successful: (%.06f %.06f %.06f)",
			info.m_hit_point.x,
			info.m_hit_point.y,
			info.m_hit_point.z);

		return info.m_hit_point;
	}

	// Something isn't good with party positioning if it's happens (which shouldn't)
	log_warning("Is there a problem with the map coords? "
		"Raycast failed: (%.06f %.06f)", x, y);
	return glm::vec3(x, 0.0f, -y);
}
