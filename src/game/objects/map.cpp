#include "map.h"

#include "core/io/file_stream_reader.h"
#include "core/graphics/renderer.h"

#include "core/managers/time.h"
#include "core/managers/nav_mesh.h"

#include "utils/profiler.h"

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

void Map::start_client()
{

}

void Map::start()
{
	const glm::vec3 map_min(-180.0f, -145.0f, -180.0f);
	const glm::vec3 map_max(180.0f, 145.0f, 180.0f);

	set_object_flag(Flags::TextureArray | Flags::GlobalTime);
	set_aabb(map_min, map_max);

	float max_vertex_x = std::numeric_limits<float>::min();
	float max_vertex_y = std::numeric_limits<float>::min();

	FileStreamReader stream {};
	stream.open("test/map.txt");

	std::vector<glm::vec3> temp_vertices {};
	uint32_t vertices_count = stream.number_from_chars<uint32_t>();
	temp_vertices.resize(vertices_count);

	for (auto& vertex : temp_vertices)
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

	std::vector<Face> faces {};
	uint32_t faces_count = stream.number_from_chars<uint32_t>();
	faces.resize(faces_count);

	std::vector<glm::vec3> nav_mesh_vertices {};

	profiler_start(generate_map);

	if (!Server::is_type(ServerType::Dedicated))
	{
		std::vector<Map::MapVertex> vertices {};
		vertices.reserve(faces_count * 3);

		for (int i = 0; i < faces_count * 3; i += 3)
		{
			int texture = stream.number_from_chars<int>();
			stream.read<std::string_view>(); // unused
			stream.read<std::string_view>(); // unused

			uint32_t x = stream.number_from_chars<uint32_t>();
			uint32_t y = stream.number_from_chars<uint32_t>();
			uint32_t z = stream.number_from_chars<uint32_t>();

			const glm::vec3 a_origin = temp_vertices[x];
			const glm::vec3 b_origin = temp_vertices[y];
			const glm::vec3 c_origin = temp_vertices[z];
			const glm::vec3 center = (a_origin + b_origin + c_origin) / 3.0f;

			if (texture != rt_water &&
				texture != rt_mountain &&
				texture != rt_river)
			{
				if (center.x > map_min.x &&
					center.y > map_min.y &&
					center.z > map_min.z &&
					center.x < map_max.x &&
					center.y < map_max.y &&
					center.z < map_max.z)
				{
					nav_mesh_vertices.push_back(a_origin);
					nav_mesh_vertices.push_back(b_origin);
					nav_mesh_vertices.push_back(c_origin);

					faces.push_back(Face{ x, y, z });
				}
			}

			if (texture > 7)
				texture = texture - 8;

			const auto build_vertex = [=](const glm::vec3& origin) -> MapVertex {
				return MapVertex { 
					origin,
					glm::vec2(origin.x / max_vertex_x, origin.z / max_vertex_y), 
					static_cast<float>(texture)
				};
			};

			MapVertex map_a_v = build_vertex(a_origin);
			MapVertex map_b_v = build_vertex(b_origin);
			MapVertex map_c_v = build_vertex(c_origin);

#ifdef _DEBUG
			setup_debug_color(texture, map_a_v, map_b_v, map_c_v);
#endif // _DEBUG

			vertices.push_back(map_a_v);
			vertices.push_back(map_b_v);
			vertices.push_back(map_c_v);
			// TODO: Calculate normal
		}

		profiler_stop(generate_map);

		add_mesh(brf::MeshBuilder::create(
			vertices,
			Buffer::Types::Static,
			brf::MeshAttribute { VertexType::Float3, cast_offset(MapVertex, m_origin) },
			brf::MeshAttribute { VertexType::Float2, cast_offset(MapVertex, m_texture) },
#ifdef _DEBUG
			brf::MeshAttribute { VertexType::Float, cast_offset(MapVertex, m_type) },
			brf::MeshAttribute { VertexType::Float3, cast_offset(MapVertex, m_color) }
#else
			brf::MeshAttribute{ VertexType::Float, cast_offset(MapVertex, m_type) }
#endif // _DEBUG
		));
		
		add_texture("test/ocean.dds", Texture2D::DDS);
		add_texture("test/mountain.dds", Texture2D::DDS);
		add_texture("test/map_steppe.dds", Texture2D::DDS);
		add_texture("test/plain.dds", Texture2D::DDS);
		add_texture("test/snow.dds", Texture2D::DDS);
		add_texture("test/desert.dds", Texture2D::DDS);

		add_texture("test/ocean.dds", Texture2D::DDS); // bridge texture ??????
		add_texture("test/ocean.dds", Texture2D::DDS); // should be river, but meh
	}
	else
	{
	}

	g_navmesh->generate(nav_mesh_vertices, faces);

	m_body.create_body(this,
		temp_vertices,
		faces,
		MotionType::Static,
		ActivationType::Activate,
		0);
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
