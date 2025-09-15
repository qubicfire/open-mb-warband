#ifndef _TERRAIN_GENERATOR_H
#define _TERRAIN_GENERATOR_H
#include "core/mb.h"

#define NUM_TERRAIN_KEYS 6
#define MIN_NUM_TERRAIN_FACES_PER_AXIS 40
#define MAX_NUM_TERRAIN_FACES_PER_AXIS 250
#define NUM_TERRAIN_LAYERS 15

enum RiverLayerType
{
	rlt_base = 2,
	rlt_expanded = 4,
};

enum TerrainLayerType
{
	tlt_rock = 0,
	tlt_earth = 1,
	tlt_green = 2,
	tlt_riverbed = 3,
};

enum RegionType
{
	rt_ocean = 0,
	rt_mountain = 1,
	rt_steppe = 2,
	rt_plain = 3,
	rt_snow = 4,
	rt_desert = 5,
	rt_bridge = 7,
	rt_river = 8,
	rt_mountain_forest = 9,
	rt_steppe_forest = 10,
	rt_forest = 11,
	rt_snow_forest = 12,
	rt_desert_forest = 13,
	rt_shore = 21,
	rt_foam = 22,
	rt_waves = 23,
};

enum GroundType
{
	ground_gray_stone = 0,
	ground_brown_stone = 1,
	ground_turf = 2,
	ground_steppe = 3,
	ground_snow = 4,
	ground_earth = 5,
	ground_desert = 6,
	ground_forest = 7,
	ground_pebbles = 8,
	ground_village = 9,
	ground_path = 10,
};

struct TerrainVertex
{
	void initialize(const glm::vec4& position);

	glm::vec4 m_position;
	float m_slope;
	unsigned int m_terrain_layer_flags;
	glm::vec4 m_normal;
	glm::vec2 m_river_direction;
	int m_river_curve;
	float m_layer_intensities[NUM_TERRAIN_LAYERS];
};

struct TerrainFace
{
	void initialize();
	
	int m_orientation;
	int m_layer_noise[2];
	glm::vec4 m_normals[2];
	float m_layer_intensities[NUM_TERRAIN_LAYERS];
};

struct TerrainLayer
{
	void set_ground_spec(int ground_spec);
	
	int m_ground_spec;
	int m_terrain_type;
};

struct TerrainGenerator
{
public:
	TerrainGenerator();

	void initialize();
	void generate();
	void generate_layers();
	void generate_terrain();
	void generate_hills(float widthFactor, float densityFactor, float heightFactor);
	void generate_hill(const glm::vec2& position, float radius, float height);
	void generate_river();

	float place_river(int* start, int* end, int terrainType, float globalDirectionFactor, float globalHeightFactor, bool modifyVertices);
	void expand_river(int pass, unsigned int srcTerrainTypeFlags, unsigned int dstTerrainTypeFlags);
	void smooth_river_height(int terrainType, float smoothFactor);
	void smooth_height();
	void compute_normals();
	void compute_vertex_layer_intensities();
	void select_face_orientation();
	void select_face_layers();
	void roughen_rock_vertices();
	void compute_face_layer_intensities();

	void set_terrain_code(const std::string& code);
	void set_shade_occluded(int value);
	void set_disable_grass(int value);
	void set_place_river(int value);
	void set_deep_water(int value);
	void set_size_x(float value);
	void set_size_y(float value);
	void set_hill_height(float value);
	void set_valley(float value);
	void set_ruggedness(int value);
	void set_vegetation(float value);
	void set_region_type(int value);
	void set_region_detail(int value);
	void set_seed(int index, int value);
	void set_terrain_type_depth(int type, float depth);

	void remove_terrain_type_flags(unsigned int flags);

	int get_disable_grass();
	int get_place_river();
	int get_deep_water();
	float get_size_x();
	float get_size_y();
	float get_hill_height();
	float get_valley();
	int get_ruggedness();
	float get_vegetation();
	int get_region_type();
	int get_region_detail();
	int get_seed(int index);
	std::string get_terrain_code();
	void get_vertices_for_face(int x, 
		int y,
		int triangleNo,
		int* vertex0,
		int* vertex1,
		int* vertex2);
	float get_random_4x();
	float get_vertex_noise(int x, int y);
public:
	unsigned int m_keys[6];
	TerrainLayer m_layers[NUM_TERRAIN_LAYERS];
	glm::vec2 m_size;
	int m_count_faces[2];
	int m_count_river_passes;
	TerrainVertex m_vertices[MAX_NUM_TERRAIN_FACES_PER_AXIS + 1][MAX_NUM_TERRAIN_FACES_PER_AXIS + 1];
	TerrainFace m_faces[MAX_NUM_TERRAIN_FACES_PER_AXIS][MAX_NUM_TERRAIN_FACES_PER_AXIS];
	float m_detail;
	float m_barrenness;
	float m_noise_frequency;
	glm::vec4 m_position_noise_randomness;
	int m_terrain_block_size;
	bool m_place_river;
};

#endif