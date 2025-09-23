#include "terrain_generator.h"
#include "terrain_utils.h"

#define bit_shift_64(x) (1ULL << (x))
#define bit_shift(x) (1 << (x))

namespace utils
{
	constexpr static auto PI = 3.14159265F;
	constexpr static auto LOG2_E = 1.44269504F;

	inline glm::vec4 cross(const glm::vec4& a, const glm::vec4& v)
	{
		return glm::vec4(a.y * v.z - a.z * v.y, a.z * v.x - a.x * v.z, a.x * v.y - a.y * v.x, 0.0f);
	}

	inline int round(float value)
	{
		float f = std::floor(value);

		return (value - f > 0.5f) ? static_cast<int>(f) + 1 : static_cast<int>(f);
	}

	inline int round_2(float value)
	{
		if (value <= 0.0f)
			return static_cast<int>((value - 0.9999999f));
		else
			return static_cast<int>(value);
	}

	float random_float()
	{
		return std::rand() % 15817 / 15817.0f;
	}

	float random_float(float max)
	{
		return random_float() * max;
	}

	float random_float(float min, float max)
	{
		return random_float() * (max - min) + min;
	}

	template <class _Tx>
	inline _Tx clamp_exclusive(_Tx value, _Tx min, _Tx max)
	{
		if (value < min)
			return min;

		if (value >= max)
			return max - 1;

		return value;
	}

	template <class _Tx>
	inline _Tx clamp_inclusive(_Tx value, _Tx min, _Tx max)
	{
		if (value < min)
			return min;

		if (value > max)
			return max;

		return value;
	}

	int random(int mod)
	{
		return mod > 1 ? (rand() % mod) : 0;
	}

	glm::vec4 normal_from_plane(const glm::vec4& p0,
		const glm::vec4& p1,
		const glm::vec4& p2)
	{
		glm::vec4 normal = utils::cross((p1 - p0), (p2 - p0));

		return glm::normalize(normal);
	}
}

static const int g_possibleMoves[9][2] =
{
	{ 0, 1 },
	{ 1, 1 },
	{ 1, 0 },
	{ 1, -1 },
	{ 0, -1 },
	{ -1, -1 },
	{ -1, 0 },
	{ -1, 1 },
	{ 0, 0 },
};

static const int g_possibleMoveDistances[8] =
{
	{ 2 },
	{ 1 },
	{ 2 },
	{ 1 },
	{ 2 },
	{ 1 },
	{ 2 },
	{ 1 },
};

void TerrainVertex::initialize(const glm::vec4& position)
{
	m_position = position;
	m_slope = 1.0f;
	m_terrain_layer_flags = 0;
	m_river_curve = -1;
	m_river_direction = glm::vec2(0.0f, 1.0f);

	for (int i = 0; i < NUM_TERRAIN_LAYERS; ++i)
	{
		m_layer_intensities[i] = 0.0f;
	}

	m_layer_intensities[tlt_rock] = 1.0f;
}

void TerrainFace::initialize()
{
	m_orientation = 0;
	m_layer_noise[0] = 0;
	m_layer_noise[1] = 0;
	m_normals[0] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	m_normals[1] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

	for (int i = 0; i < NUM_TERRAIN_LAYERS; ++i)
	{
		m_layer_intensities[i] = 0;
	}
}

void TerrainLayer::set_ground_spec(int ground_spec)
{
	m_ground_spec = ground_spec;

	if (ground_spec >= 0)
		m_terrain_type = -1;
}

TerrainGenerator::TerrainGenerator()
{
	m_detail = 2.0f;
	m_terrain_block_size = 40;
	m_place_river = false;
	m_count_river_passes = 2;
	initialize();
}

void TerrainGenerator::initialize()
{
	for (int i = 0; i < NUM_TERRAIN_LAYERS; ++i)
	{
		m_layers[i].m_ground_spec = ground_gray_stone;
		m_layers[i].m_terrain_type = 0;
	}
}

std::string TerrainGenerator::get_terrain_code()
{
	char buffer[51];

	sprintf_s(buffer, "0x%.8x%.8x%.8x%.8x%.8x%.8x", m_keys[5], m_keys[4], m_keys[3], m_keys[2], m_keys[1], m_keys[0]);
	return buffer;
}

void TerrainGenerator::set_terrain_code(const std::string& code)
{
	for (int i = 0; i < NUM_TERRAIN_KEYS; i++)
		m_keys[i] = 0;

	int start_origin = 0;
	int key = 0;

	if (code[0] == '0' && (code[1] == 'x' || code[1] == 'X'))
		start_origin = 2;

	for (int i = code.length(); i >= start_origin && key < NUM_TERRAIN_KEYS; i -= 8)
	{
		int start = std::max(i - 8, start_origin);
		std::string key_code = code.substr(start, i - start);
		unsigned int key_value = 0;

		for (size_t j = 0; j < key_code.length(); ++j)
		{
			char code = key_code[j];
			int value = 0;

			if (code - '0' <= 9)
				value = code - '0';
			else if (code - 'a' <= 5)
				value = code - 'W';
			else if (code - 'A' <= 5)
				value = code - '7';

			key_value = key_value * 16 + value;
		}

		m_keys[key++] = key_value;
	}
}

void TerrainGenerator::set_shade_occluded(int value)
{
	m_keys[3] &= ~(0x1 << 30);
	m_keys[3] |= (std::min(value, 1) << 30);
}

void TerrainGenerator::set_disable_grass(int value)
{
	m_keys[5] &= ~(0x3 << 2);
	m_keys[5] |= (std::min(value, 3) << 2);
}

void TerrainGenerator::set_place_river(int value)
{
	m_keys[3] &= ~(0x1 << 31);
	m_keys[3] |= (std::min(value, 1) << 31);
}

void TerrainGenerator::set_deep_water(int value)
{
	m_keys[1] &= ~(0x1 << 31);
	m_keys[1] |= (std::min(value, 1) << 31);
}

void TerrainGenerator::set_size_x(float value)
{
	m_keys[3] &= ~(0x3FF << 0);
	m_keys[3] |= (std::min((int)value, 1023) << 0);
}

void TerrainGenerator::set_size_y(float value)
{
	m_keys[3] &= ~(0x3FF << 10);
	m_keys[3] |= (std::min((int)value, 1023) << 10);
}

void TerrainGenerator::set_hill_height(float value)
{
	m_keys[4] &= ~(0x7F << 7);
	m_keys[4] |= (utils::clamp_exclusive(utils::round_2(value), 0, 128) << 7);
}

void TerrainGenerator::set_valley(float value)
{
	m_keys[4] &= ~(0x7F << 0);
	m_keys[4] |= (utils::clamp_exclusive(utils::round(value * 100.0f), 0, 100) << 0);
}

void TerrainGenerator::set_ruggedness(int value)
{
	m_keys[4] &= ~(0x7F << 14);
	m_keys[4] |= (std::min(value, 127) << 14);
}

void TerrainGenerator::set_vegetation(float value)
{
	m_keys[4] &= ~(0x7F << 21);
	m_keys[4] |= (utils::clamp_exclusive(utils::round(value * 100.0f), 0, 100) << 21);
}

void TerrainGenerator::set_region_type(int value)
{
	m_keys[4] &= ~(0xF << 28);
	m_keys[4] |= (std::min(value, 16) << 28);
}

void TerrainGenerator::set_region_detail(int value)
{
	m_keys[5] &= ~(0x3 << 0);
	m_keys[5] |= (std::min(value, 3) << 0);
}

void TerrainGenerator::set_seed(int index, int value)
{
	m_keys[index] &= ~0x7FFFFFFF;
	m_keys[index] |= std::min(value, 0x7FFFFFFF);
}

int TerrainGenerator::get_disable_grass()
{
	return (m_keys[5] >> 2) & 0x3;
}

int TerrainGenerator::get_place_river()
{
	return (m_keys[3] >> 31) & 0x1;
}

int TerrainGenerator::get_deep_water()
{
	return (m_keys[1] >> 31) & 0x1;
}

float TerrainGenerator::get_size_x()
{
	return (float)((m_keys[3] >> 0) & 0x3FF);
}

float TerrainGenerator::get_size_y()
{
	return (float)((m_keys[3] >> 10) & 0x3FF);
}

float TerrainGenerator::get_hill_height()
{
	return (float)((m_keys[4] >> 7) & 0x7F);
}

float TerrainGenerator::get_valley()
{
	return (float)((m_keys[4] >> 0) & 0x7F) / 100.0f;
}

int TerrainGenerator::get_ruggedness()
{
	return (m_keys[4] >> 14) & 0x7F;
}

float TerrainGenerator::get_vegetation()
{
	return (float)((m_keys[4] >> 21) & 0x7F) / 100.0f;
}

int TerrainGenerator::get_region_type()
{
	return (m_keys[4] >> 28) & 0xF;
}

int TerrainGenerator::get_region_detail()
{
	return (m_keys[5] >> 0) & 0x3;
}

int TerrainGenerator::get_seed(int index)
{
	return (m_keys[index] >> 0) & 0x7FFFFFFF;
}

void TerrainGenerator::generate()
{
	initialize();
	m_detail = get_region_detail() + 2.0f;
	srand(get_seed(0));
	m_place_river = get_place_river() != 0;
	m_size.x = get_size_x();
	m_size.y = get_size_y();
	m_count_faces[0] = utils::clamp_inclusive((int)(m_size.x / m_detail), MIN_NUM_TERRAIN_FACES_PER_AXIS, MAX_NUM_TERRAIN_FACES_PER_AXIS);
	m_count_faces[1] = utils::clamp_inclusive((int)(m_size.y / m_detail), MIN_NUM_TERRAIN_FACES_PER_AXIS, MAX_NUM_TERRAIN_FACES_PER_AXIS);
	m_size.x = m_count_faces[0] * m_detail;
	m_size.y = m_count_faces[1] * m_detail;
	generate_layers();

	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
			m_vertices[x][y].initialize(glm::vec4(x * m_detail, y * m_detail, 0.0f, 0.0f));
	}

	for (int y = 0; y < m_count_faces[1]; ++y)
	{
		for (int x = 0; x < m_count_faces[0]; ++x)
			m_faces[x][y].initialize();
	}

	generate_terrain();
	std::srand(get_seed(1));
	generate_river();
	smooth_height();
	compute_normals();
	compute_vertex_layer_intensities();
	select_face_orientation();
	select_face_layers();
	roughen_rock_vertices();
	std::srand(get_seed(2));
	compute_normals();
	compute_face_layer_intensities();
}

void TerrainGenerator::generate_layers()
{
	for (int i = 0; i < NUM_TERRAIN_LAYERS; ++i)
		m_layers[i].m_ground_spec = -1;

	int random = rand() % 2;

	if (random == 1)
		m_layers[tlt_rock].set_ground_spec(ground_brown_stone);
	else if (random == 0)
		m_layers[tlt_rock].set_ground_spec(ground_gray_stone);

	rand();
	m_barrenness = 0.19f;

	int earth_ground_spec = ground_earth;
	int green_ground_spec = ground_turf;
	int multitex_ground_spec = -1;

	switch (get_region_type())
	{
	case rt_plain:
		earth_ground_spec = ground_earth;
		green_ground_spec = ground_turf;
		multitex_ground_spec = ground_turf;
		break;
	case rt_steppe:
		earth_ground_spec = ground_earth;
		green_ground_spec = ground_steppe;
		multitex_ground_spec = ground_steppe;
		rand();
		break;
	case rt_snow:
	case rt_snow_forest:
		earth_ground_spec = ground_snow;
		green_ground_spec = -1;
		multitex_ground_spec = -1;
		break;
	case rt_desert:
	case rt_desert_forest:
		earth_ground_spec = ground_desert;
		green_ground_spec = -1;
		multitex_ground_spec = -1;
		m_barrenness = 0.26f;
		break;
	case rt_forest:
		earth_ground_spec = ground_forest;
		green_ground_spec = ground_turf;
		multitex_ground_spec = ground_forest;
		break;
	case rt_steppe_forest:
		earth_ground_spec = ground_forest;
		green_ground_spec = ground_steppe;
		multitex_ground_spec = -1;
		break;
	}

	m_layers[tlt_earth].set_ground_spec(earth_ground_spec);
	m_layers[tlt_green].set_ground_spec(green_ground_spec);
	m_layers[tlt_riverbed].set_ground_spec(ground_pebbles);
}

void TerrainGenerator::generate_terrain()
{
	float size_factor = (m_size[1] + m_size[0]) * 0.5f * (utils::random_float(1.0f, 1.5f));
	m_noise_frequency = (m_size[1] + m_size[0]) * 0.2f * (utils::random_float(1.0f, 1.5f));

	float valley = get_valley();
	glm::vec4 position_randomness {};

	position_randomness.x = utils::random_float(10000.0f);
	position_randomness.y = utils::random_float(10000.0f);
	position_randomness.z = utils::random_float(10000.0f);

	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
		{
			glm::vec4 noise = perlin_octave(m_noise_frequency, 0.6f, 1, m_vertices[x][y].m_position + position_randomness);
			float slope_factor = std::clamp((noise.x + noise.y + noise.z) * valley * 9.0f, 0.0f, 1.0f);

			m_vertices[x][y].m_slope = (cos(slope_factor * utils::PI) + 1.0f) * 0.45f + 0.1f;
		}
	}

	float altitude_randomness = utils::random_float(7.0f, 10.0f);

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 13; ++j)
		{
			if (m_count_faces[1 - i] >= 0)
			{
				float altitude_factor = sin((1.0f - j / 13.0f) * 3.2f) * altitude_randomness;

				for (int k = 0; k <= m_count_faces[1 - i]; ++k)
				{
					int x1;
					int y1;
					int x2;
					int y2;

					if (i == 0)
					{
						x1 = j;
						x2 = m_count_faces[0] - j;
						y1 = k;
						y2 = k;
					}
					else
					{
						x1 = k;
						x2 = k;
						y1 = j;
						y2 = m_count_faces[1] - j;
					}

					TerrainVertex* vertex_a = &m_vertices[x1][y1];
					glm::vec4 noise_a = perlin_octave(40.0f, 0.6f, 4, vertex_a->m_position + position_randomness);
					float altitude_a = (noise_a.z + noise_a.z + 0.4f) * altitude_factor;

					if (altitude_a > vertex_a->m_position.z)
						vertex_a->m_position.z = altitude_a;

					TerrainVertex* vertex_b = &m_vertices[x2][y2];
					glm::vec4 noise_b = perlin_octave(40.0f, 0.6f, 4, vertex_b->m_position + position_randomness);
					float altitude_b = (noise_b.z + noise_b.z + 0.4f) * altitude_factor;

					if (altitude_b > vertex_b->m_position.z)
						vertex_b->m_position.z = altitude_b;
				}
			}
		}
	}

	float hill_factor = std::max(size_factor / (pow(2.0f, (100 - utils::clamp_exclusive(get_ruggedness(), 0, 100)) * 0.045f) * 4.0f), 2.0f);
	int num_passes = (int)(log(hill_factor) * utils::LOG2_E);
	float height_factor = 1.0f;

	for (int i = 0; i <= num_passes; ++i)
	{
		float density_factor;

		if (i == num_passes)
			density_factor = (hill_factor - bit_shift_64(num_passes)) / bit_shift_64(num_passes);
		else
			density_factor = 1.0f;

		generate_hills(size_factor / bit_shift_64(i), density_factor, height_factor);
		height_factor -= 0.05f;
	}

	float min_height = 10000.0f;

	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
			min_height = std::min(min_height, m_vertices[x][y].m_position.z);
	}

	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
			m_vertices[x][y].m_position.z -= min_height;
	}
}

void TerrainGenerator::generate_hills(float width_factor, 
	float density_factor,
	float height_factor)
{
	int num_hills = (int)(utils::round(m_size[0] * density_factor * m_size[1] / (width_factor * width_factor * 0.25f)) * utils::random_float(0.4f, 0.6f) * 30.0f);

	for (int i = 0; i < num_hills; ++i)
	{
		float radius = (get_random_4x() * 0.5f + 0.05f) * width_factor;
		glm::vec2 position {};

		position.y = (m_size[1] + (width_factor * 2)) * utils::random_float() - width_factor;
		position.x = (m_size[0] + (width_factor * 2)) * utils::random_float() - width_factor;
		generate_hill(position, radius, utils::random_float(0.2f, 0.3f) * radius * height_factor);
	}
}

void TerrainGenerator::generate_hill(const glm::vec2& position, float radius, float height)
{
	float hill_height = get_hill_height();
	int min[2];
	int max[2];

	for (int i = 0; i < 2; ++i)
	{
		min[i] = std::max(utils::round(std::ceil((position[i] - radius) / m_detail)), 0);
		max[i] = std::min(utils::round(std::floor((position[i] + radius) / m_detail)), m_count_faces[i]);
	}

	for (int y = min[1]; y <= max[1]; ++y)
	{
		for (int x = min[0]; x <= max[0]; ++x)
		{
			TerrainVertex* vertex = &m_vertices[x][y];
			float distance = (glm::vec2((float)x, (float)y) * m_detail - position).length() / radius;

			if (distance < 1.0f)
				vertex->m_position.z += ((std::cos(distance * utils::PI) + 1.0f) * 0.5f * height * vertex->m_slope) * hill_height * 0.01f;
		}
	}
}

void TerrainGenerator::generate_river()
{
	bool river_placed = false;
	int start[2];
	int end[2];

	start[0] = 0;
	start[1] = 0;

	if (m_place_river)
	{
		for (int i = 0; i < 4 && !river_placed; ++i)
		{
			int tries = 20;
			int x;
			int y;
			float best_noise = -10000.0f;

			do
			{
				if (utils::random_float() >= 0.5f)
				{
					x = utils::random(m_count_faces[0] - 3) + 3;

					y = rand() % 4 + 3;

					if (utils::random_float() < 0.5f)
						y = m_count_faces[1] - y;
				}
				else
				{
					x = rand() % 4 + 3;

					if (utils::random_float() < 0.5f)
						x = m_count_faces[0] - x;

					y = utils::random(m_count_faces[1] - 3) + 3;
				}

				float noise = get_vertex_noise(start[0], start[1]);

				if (noise >= best_noise)
				{
					best_noise = noise;
					start[0] = x;
					start[1] = y;
				}
			} while (--tries);

			bool end_found = false;

			tries = 50;

			while (tries && !end_found)
			{
				if (utils::random_float() >= 0.5f)
				{
					end[1] = 0;

					if (utils::random_float() < 0.5f)
						end[1] = m_count_faces[1] + 1;

					end[0] = utils::random(m_count_faces[0] + 1);
				}
				else
				{
					end[0] = 0;

					if (utils::random_float() < 0.5f)
						end[0] = m_count_faces[0] + 1;

					end[1] = utils::random(m_count_faces[1] + 1);
				}

				int distance_X = end[0] - start[0];

				if (distance_X < 0)
					distance_X = -distance_X;

				int distance_Y = end[1] - start[1];

				if (distance_Y < 0)
					distance_Y = -distance_Y;

				if ((m_count_faces[0] + m_count_faces[1]) * 0.7f < (distance_X + distance_Y))
					end_found = true;

				--tries;
			}

			if (end_found)
			{
				if (place_river(start, end, rlt_base, 1.5f, 0.9f, true) >= 3.5f)
					remove_terrain_type_flags(bit_shift(rlt_base) | bit_shift(rlt_expanded));
				else
					river_placed = true;
			}
		}
	}

	m_count_river_passes = rand() % 3;

	if (m_detail >= 2.1f && m_count_river_passes > 1)
		m_count_river_passes = 1;

	if (m_detail > 3.1f)
		m_count_river_passes = 0;

	if (get_deep_water())
		m_count_river_passes = 0;

	if (river_placed)
	{
		expand_river(1, bit_shift(rlt_base), bit_shift(rlt_base) | bit_shift(rlt_expanded));

		for (int i = 0; i < m_count_river_passes; ++i)
			expand_river(2, bit_shift(rlt_base), bit_shift(rlt_base) | bit_shift(rlt_expanded));

		expand_river(2, bit_shift(rlt_base), bit_shift(rlt_expanded));
		expand_river(2, bit_shift(rlt_expanded), bit_shift(rlt_expanded));
		set_terrain_type_depth(rlt_base, get_deep_water() ? -2.3f : -1.3f);
		smooth_river_height(rlt_base, 0.5f);
	}
}

float TerrainGenerator::place_river(int* start,
	int* end, 
	int terrain_type,
	float global_direction_factor,
	float global_height_factor,
	bool modify_vertices)
{
	float depth = 10000.0f;
	glm::vec2 direction((float)(end[0] - start[0]), (float)(end[1] - start[1]));
	int current_x = start[0];
	int current_y = start[1];
	int curves_count = 0;
	glm::vec2 possible_directions[8];

	direction = glm::normalize(direction);

	for (int i = 0; i < 8; ++i)
	{
		possible_directions[i] = glm::vec2(
			(float)g_possibleMoves[i][0], 
			(float)g_possibleMoves[i][1]
		);

		possible_directions[i] = glm::normalize(possible_directions[i]);
	}

	int terrain_type_flags = bit_shift(rlt_base);
	TerrainVertex* start_vertex = &m_vertices[start[0]][start[1]];
	TerrainVertex* current_vertex = start_vertex;

	start_vertex->m_terrain_layer_flags |= terrain_type_flags;

	if (modify_vertices)
		start_vertex->m_river_direction = direction;

	bool at_end = false;
	float hill_height = get_hill_height();
	float direction_like_liness[8];

	while (true)
	{
		glm::vec2 current_direction((float)(end[0] - current_x), (float)(end[1] - current_y));

		current_direction = glm::normalize(current_direction);

		for (int i = 0; i < 8; ++i)
		{
			direction_like_liness[i] = 0.0f;

			int next_x = current_x + g_possibleMoves[i][0];
			int next_y = current_y + g_possibleMoves[i][1];
			float placement_factor = 0.0f;
			int distance_from_bounds = std::max(20 - next_x, 0) + std::max(next_x + 20 - m_count_faces[0], 0) + std::max(20 - next_y, 0) + std::max(next_y + 20 - m_count_faces[1], 0);

			if (distance_from_bounds > 0)
			{
				if (next_x <= 20 && possible_directions[i].x <= 0.0f ||
					next_y <= 20 && possible_directions[i].y <= 0.0f ||
					next_x >= m_count_faces[0] - 20 && possible_directions[i].x >= 0.0f ||
					next_y >= m_count_faces[1] - 20 && possible_directions[i].y >= 0.0f)
				{
					placement_factor = std::clamp((float)distance_from_bounds, 0.0f, 24.0f) * -0.04f;
				}
			}

			if (next_x < 0 || next_x > m_count_faces[0] || next_y < 0 || next_y > m_count_faces[1])
			{
				direction_like_liness[i] = (glm::dot(possible_directions[i], direction) + 1.0f) / 2;
			}
			else
			{
				TerrainVertex* next_vertex = &m_vertices[next_x][next_y];

				if (!(next_vertex->m_terrain_layer_flags & terrain_type_flags))
				{
					float height_factor = (current_vertex->m_position.z - next_vertex->m_position.z) * global_height_factor * 3.0f;

					if (height_factor < 0.0f)
						height_factor *= (0.25f - hill_height * 0.0022f);

					direction_like_liness[i] = height_factor + ((glm::dot(direction, possible_directions[i]) + 0.7f) * 0.5f * global_direction_factor) + ((glm::dot(current_direction, possible_directions[i]) + 0.7f) * 0.5f) * 0.65f + placement_factor;
				}
				else
				{
					if (next_vertex->m_river_curve < curves_count - 30)
						at_end = true;

					direction_like_liness[i] = -1.0f;
				}
			}
		}

		for (int i = 0; i < 8; ++i)
		{
			if (direction_like_liness[i] > 0.0f)
				direction_like_liness[i] = direction_like_liness[i] * direction_like_liness[i] * direction_like_liness[i];
		}

		float threshold = 0.0f;

		for (int i = 0; i < 8; ++i)
		{
			if (direction_like_liness[i] > 0.0f)
				threshold += direction_like_liness[i];
		}

		threshold *= utils::random_float();

		float total_like_liness = 0.0f;
		int selected_move = -1;

		for (int i = 0; i < 8; ++i)
		{
			if (direction_like_liness[i] > 0.0f)
			{
				if (total_like_liness <= threshold && direction_like_liness[i] + total_like_liness > threshold)
					selected_move = i;

				total_like_liness += direction_like_liness[i];
			}
		}

		if (selected_move != -1)
		{
			int new_x = current_x + g_possibleMoves[selected_move][0];

			if (new_x >= 3 && new_x <= m_count_faces[0] - 3)
			{
				int new_y = current_y + g_possibleMoves[selected_move][1];

				if (new_y >= 3 && new_y <= m_count_faces[1] - 3)
				{
					if (direction_like_liness[selected_move] > 0.0f)
					{
						current_x = new_x;
						current_y = new_y;
						direction = direction * 0.9f + possible_directions[selected_move] * 0.1f;
						direction = glm::normalize(direction);
						curves_count++;
						current_vertex = &m_vertices[current_x][current_y];
						current_vertex->m_terrain_layer_flags |= terrain_type_flags;
						current_vertex->m_river_curve = curves_count;

						if (modify_vertices)
						{
							current_vertex->m_river_direction = direction;
						}

						depth = std::min(depth, current_vertex->m_position.z);

						if (curves_count != 100000 && !at_end)
							continue;
					}
				}
			}
		}

		break;
	}

	return depth;
}

void TerrainGenerator::expand_river(int pass, unsigned int srcTerrainTypeFlags, unsigned int terrain_type_flags)
{
	int modified_vertices[MAX_NUM_TERRAIN_FACES_PER_AXIS + 1][MAX_NUM_TERRAIN_FACES_PER_AXIS + 1];
	int directions_count;
	int directions[8];

	if (pass == 1)
	{
		directions_count = 4;
		directions[0] = 2;
		directions[1] = 4;
		directions[2] = 5;
		directions[3] = 6;
	}
	else
	{
		directions_count = 8;
		directions[0] = 0;
		directions[1] = 1;
		directions[2] = 2;
		directions[3] = 3;
		directions[4] = 4;
		directions[5] = 5;
		directions[6] = 6;
		directions[7] = 7;
	}

	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
			modified_vertices[x][y] = 0;
	}

	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
		{
			glm::vec2 average_river_direction {};
			int matches_count = 0;
			TerrainVertex* vertex = &m_vertices[x][y];

			for (int i = 0; i < directions_count; ++i)
			{
				int possible_x = x + g_possibleMoves[directions[i]][0];
				int possible_y = y + g_possibleMoves[directions[i]][1];

				if (possible_x >= 0 && possible_x <= m_count_faces[0])
				{
					if (possible_y >= 0 && possible_y <= m_count_faces[1])
					{
						TerrainVertex* possible_vertex = &m_vertices[possible_x][possible_y];

						if (possible_vertex->m_terrain_layer_flags & srcTerrainTypeFlags)
						{
							matches_count++;
							average_river_direction += possible_vertex->m_river_direction;
						}
					}
				}
			}

			if (matches_count)
			{
				modified_vertices[x][y] = 1;
				vertex->m_river_direction = average_river_direction / (float)matches_count;
				vertex->m_river_direction = glm::normalize(vertex->m_river_direction);
			}
		}
	}

	if (terrain_type_flags)
	{
		for (int y = 0; y <= m_count_faces[1]; ++y)
		{
			for (int x = 0; x <= m_count_faces[0]; ++x)
			{
				if (modified_vertices[x][y])
					m_vertices[x][y].m_terrain_layer_flags |= terrain_type_flags;
			}
		}
	}
}

void TerrainGenerator::smooth_river_height(int terrain_type, float smooth_factor)
{
	unsigned int flag = bit_shift(terrain_type);
	float height;

	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
		{
			TerrainVertex* vertex = &m_vertices[x][y];

			if (!(vertex->m_terrain_layer_flags & terrain_type))
			{
				int smooth_type = 0;

				for (int i = 0; i < 8; ++i)
				{
					int new_x = x + g_possibleMoves[i][0];
					int new_y = y + g_possibleMoves[i][1];

					if (new_x >= 0 && new_x <= m_count_faces[0])
					{
						if (new_y >= 0 && new_y <= m_count_faces[1])
						{
							if (m_vertices[new_x][new_y].m_terrain_layer_flags & flag)
							{
								int newType = g_possibleMoveDistances[i];

								if (newType == 2 || (newType == 1 && smooth_type == 0))
									smooth_type = newType;

								height = m_vertices[new_x][new_y].m_position.z;
							}
						}
					}
				}

				if (smooth_type)
				{
					float factor = smooth_factor;

					if (smooth_type == 1)
						factor *= 0.7f;

					vertex->m_position.z += (height - vertex->m_position.z) * factor;
				}
			}
		}
	}
}

void TerrainGenerator::smooth_height()
{
	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
		{
			TerrainVertex* vertex = &m_vertices[x][y];
			float height_sum = vertex->m_position.z;
			float smooth_amount = 1.0f;

			for (int i = 0; i < 8; ++i)
			{
				int new_x = x + g_possibleMoves[i][0];
				int new_y = y + g_possibleMoves[i][1];

				if (new_x >= 0 && new_x <= m_count_faces[0])
				{
					if (new_y >= 0 && new_y <= m_count_faces[1])
					{
						height_sum += m_vertices[new_x][new_y].m_position.z;
						smooth_amount += 1.0f;
					}
				}
			}

			float height_difference = std::clamp((height_sum / smooth_amount) - vertex->m_position.z, -10.0f, 2.0f);

			vertex->m_position.z += utils::random_float(0.2f, 0.6f) * height_difference;
		}
	}
}

void TerrainGenerator::compute_normals()
{
	for (int y = 0; y < m_count_faces[1]; ++y)
	{
		for (int x = 0; x < m_count_faces[0]; ++x)
		{
			TerrainFace* face = &m_faces[x][y];

			for (int i = 0; i < 2; ++i)
			{
				int vertex_a[2];
				int vertex_b[2];
				int vertex_c[2];

				get_vertices_for_face(x, y, i, vertex_a, vertex_b, vertex_c);
				face->m_normals[i] = utils::normal_from_plane(m_vertices[vertex_a[0]][vertex_a[1]].m_position, m_vertices[vertex_b[0]][vertex_b[1]].m_position, m_vertices[vertex_c[0]][vertex_c[1]].m_position);
			}
		}
	}

	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
			m_vertices[x][y].m_normal = {};
	}

	for (int y = 0; y < m_count_faces[1]; ++y)
	{
		for (int x = 0; x < m_count_faces[0]; ++x)
		{
			TerrainFace* face = &m_faces[x][y];

			for (int i = 0; i < 2; ++i)
			{
				int vertex_a[2];
				int vertex_b[2];
				int vertex_c[2];

				get_vertices_for_face(x, y, i, vertex_a, vertex_b, vertex_c);
				m_vertices[vertex_a[0]][vertex_a[1]].m_normal += face->m_normals[i];
				m_vertices[vertex_b[0]][vertex_b[1]].m_normal += face->m_normals[i];
				m_vertices[vertex_c[0]][vertex_c[1]].m_normal += face->m_normals[i];
			}
		}
	}

	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
			m_vertices[x][y].m_normal = glm::normalize(m_vertices[x][y].m_normal);
	}
}

void TerrainGenerator::compute_vertex_layer_intensities()
{
	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
		{
			TerrainVertex* vertex = &m_vertices[x][y];

			if ((m_layers[tlt_earth].m_terrain_type == -1 || vertex->m_terrain_layer_flags & bit_shift(m_layers[tlt_earth].m_terrain_type)) && vertex->m_position.z <= 10000.0f)
				vertex->m_layer_intensities[tlt_earth] = 1.0f - std::clamp(((1.0f - vertex->m_normal.z) - m_barrenness) * 12.0f, 0.0f, 1.0f);

			if (m_layers[tlt_green].m_ground_spec >= 0)
			{
				glm::vec4 noise = perlin_octave(15.0f, 0.6f, 3, glm::vec4((float)x, (float)y, 0.0f, 0.0f));
				float intensity = std::clamp((noise.x + noise.y + noise.z) * 4.5f + 0.7f, 0.0f, 1.0f);

				vertex->m_layer_intensities[tlt_green] = vertex->m_layer_intensities[tlt_earth] * intensity;

				if (intensity > 0.99f)
					vertex->m_layer_intensities[tlt_earth] = 0.0f;
			}

			if (m_place_river && vertex->m_position.z < 0.0f)
				vertex->m_layer_intensities[tlt_riverbed] = 1.0f;
		}
	}
}

void TerrainGenerator::select_face_orientation()
{
	int possible_directions[] = { 8, 2, 1, 0 };
	int layer_noise[4];

	for (int y = 0; y < m_count_faces[1]; ++y)
	{
		for (int x = 0; x < m_count_faces[0]; ++x)
		{
			TerrainFace* face = &m_faces[x][y];

			for (int i = 0; i < 4; ++i)
			{
				int new_x = x + g_possibleMoves[possible_directions[i]][0];
				int new_y = y + g_possibleMoves[possible_directions[i]][1];

				if (new_x >= 0 && new_x <= m_count_faces[0] && new_y >= 0 && new_y <= m_count_faces[1])
				{
					TerrainVertex* new_vertex = &m_vertices[new_x][new_y];

					layer_noise[i] = 0;

					for (int j = 1; j < NUM_TERRAIN_LAYERS; ++j)
					{
						if (new_vertex->m_layer_intensities[j] > 0.4f)
							layer_noise[i] = j;
					}
				}
				else
				{
					layer_noise[i] = -1;
				}
			}

			int inverts_count = -1;

			if (layer_noise[1] != layer_noise[0] && layer_noise[3] != layer_noise[0])
				inverts_count = 0;

			if (layer_noise[2] != layer_noise[1] && layer_noise[0] != layer_noise[1])
				inverts_count = 1;

			if (layer_noise[3] != layer_noise[2] && layer_noise[1] != layer_noise[2])
				inverts_count = 2;

			if (layer_noise[0] == layer_noise[3] || layer_noise[2] == layer_noise[3])
			{
				if (inverts_count == -1)
				{
					glm::vec4 up_right = m_vertices[x + 1][y + 1].m_position - m_vertices[x][y].m_position;
					glm::vec4 right = m_vertices[x + 1][y].m_position - m_vertices[x][y].m_position;
					glm::vec4 up = m_vertices[x][y + 1].m_position - m_vertices[x][y].m_position;

					if (glm::dot(up_right, utils::cross(right, up)) > 0.0f)
						face->m_orientation = 0;
					else
						face->m_orientation = 1;
				}
				else if (inverts_count == 0 || inverts_count == 2)
				{
					face->m_orientation = 0;
				}
				else
				{
					face->m_orientation = 1;
				}
			}
			else
			{
				face->m_orientation = 1;
			}
		}
	}
}

void TerrainGenerator::select_face_layers()
{
	int possible_directions[] = { 8, 2, 1, 0 };
	int layer_noise[4];

	for (int y = 0; y < m_count_faces[1]; ++y)
	{
		for (int x = 0; x < m_count_faces[0]; ++x)
		{
			TerrainFace* face = &m_faces[x][y];

			for (int i = 0; i < 4; ++i)
			{
				int new_x = x + g_possibleMoves[possible_directions[i]][0];
				int new_y = y + g_possibleMoves[possible_directions[i]][1];

				if (new_x >= 0 && new_x <= m_count_faces[0] && new_y >= 0 && new_y <= m_count_faces[1])
				{
					TerrainVertex* newVertex = &m_vertices[new_x][new_y];

					layer_noise[i] = 0;

					for (int j = 1; j < NUM_TERRAIN_LAYERS; ++j)
					{
						if (newVertex->m_layer_intensities[j] > 0.4f)
							layer_noise[i] = j;
					}
				}
				else
				{
					layer_noise[i] = -1;
				}
			}

			if (face->m_orientation)
			{
				if (layer_noise[0] > 0)
					face->m_layer_noise[1] = layer_noise[0];
				else if (layer_noise[2] > 0)
					face->m_layer_noise[1] = layer_noise[2];
				else
					face->m_layer_noise[1] = layer_noise[3];

				if (layer_noise[0] > 0)
					face->m_layer_noise[0] = layer_noise[0];
				else if (layer_noise[1] > 0)
					face->m_layer_noise[0] = layer_noise[1];
				else
					face->m_layer_noise[0] = layer_noise[2];
			}
			else
			{
				if (layer_noise[1] > 0)
					face->m_layer_noise[1] = layer_noise[1];
				else if (layer_noise[2] > 0)
					face->m_layer_noise[1] = layer_noise[2];
				else
					face->m_layer_noise[1] = layer_noise[3];

				if (layer_noise[1] > 0)
					face->m_layer_noise[0] = layer_noise[1];
				else if (layer_noise[3] > 0)
					face->m_layer_noise[0] = layer_noise[3];
				else
					face->m_layer_noise[0] = layer_noise[0];
			}
		}
	}
}

void TerrainGenerator::roughen_rock_vertices()
{
	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
		{
			TerrainVertex* vertex = &m_vertices[x][y];

			if (vertex->m_layer_intensities[tlt_green] + vertex->m_layer_intensities[tlt_earth] < 0.5f)
			{
				float increase = utils::random_float(-0.5f, 0.5f) * (1.0f - vertex->m_layer_intensities[tlt_earth]) * m_detail * 0.7f;

				vertex->m_position.z += increase;
			}
		}
	}
}

void TerrainGenerator::compute_face_layer_intensities()
{
	int possible_directions[] = { 8, 2, 1, 0 };
	int layer_noise[4];

	for (int y = 0; y < m_count_faces[1]; ++y)
	{
		for (int x = 0; x < m_count_faces[0]; ++x)
		{
			TerrainFace* face = &m_faces[x][y];
			int green_layers_count = 0;
			float green_layer_intensity = 0.0f;

			for (int i = 0; i < 4; ++i)
			{
				int new_x = x + g_possibleMoves[possible_directions[i]][0];
				int new_y = y + g_possibleMoves[possible_directions[i]][1];
				TerrainVertex* new_vertex = &m_vertices[new_x][new_y];

				if (new_x >= 0 && new_x <= m_count_faces[0] && new_y >= 0 && new_y <= m_count_faces[1])
				{
					layer_noise[i] = 0;

					for (int j = 1; j < NUM_TERRAIN_LAYERS; ++j)
					{
						if (new_vertex->m_layer_intensities[j] > 0.4f)
							layer_noise[i] = j;
					}
				}
				else
				{
					layer_noise[i] = -1;
				}

				if (layer_noise[i] == tlt_green)
					green_layers_count++;

				green_layer_intensity += new_vertex->m_layer_intensities[tlt_green];

				float intensity = 1.0f;

				for (int j = NUM_TERRAIN_LAYERS - 1; j >= 0; --j)
				{
					float val = new_vertex->m_layer_intensities[j] * intensity;

					face->m_layer_intensities[j] += val * 0.25f;
					intensity -= val;
				}
			}
		}
	}
}

void TerrainGenerator::get_vertices_for_face(int x,
	int y, 
	int no_triangles, 
	int* vertex_a, 
	int* vertex_b,
	int* vertex_c)
{
	TerrainFace* face = &m_faces[x][y];

	if (face->m_orientation)
	{
		vertex_a[0] = x;
		vertex_a[1] = y;

		if (no_triangles == 0)
		{
			vertex_b[0] = x + 1;
			vertex_b[1] = y;
			vertex_c[0] = x + 1;
			vertex_c[1] = y + 1;
		}
		else
		{
			vertex_b[0] = x + 1;
			vertex_b[1] = y + 1;
			vertex_c[0] = x;
			vertex_c[1] = y + 1;
		}
	}
	else
	{
		vertex_a[0] = x + 1;
		vertex_a[1] = y;

		if (no_triangles == 0)
		{
			vertex_b[0] = x;
			vertex_b[1] = y + 1;
			vertex_c[0] = x;
			vertex_c[1] = y;
		}
		else
		{
			vertex_b[0] = x + 1;
			vertex_b[1] = y + 1;
			vertex_c[0] = x;
			vertex_c[1] = y + 1;
		}
	}
}

float TerrainGenerator::get_random_4x()
{
	return (utils::random_float() 
		+ utils::random_float()
		+ utils::random_float()
		+ utils::random_float()) / 4;
}

float TerrainGenerator::get_vertex_noise(int x, int y)
{
	glm::vec4 noise = perlin_octave(m_noise_frequency, 0.5f, 1, m_position_noise_randomness + m_vertices[x][y].m_position);

	return noise.x + noise.y + noise.z;
}

void TerrainGenerator::remove_terrain_type_flags(unsigned int flags)
{
	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
			m_vertices[x][y].m_terrain_layer_flags &= ~flags;
	}
}

void TerrainGenerator::set_terrain_type_depth(int type, float depth)
{
	for (int y = 0; y <= m_count_faces[1]; ++y)
	{
		for (int x = 0; x <= m_count_faces[0]; ++x)
		{
			if (m_vertices[x][y].m_terrain_layer_flags & bit_shift(type))
				m_vertices[x][y].m_position.z = depth;
		}
	}
}