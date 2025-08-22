#ifndef _BRF_MESH_H
#define _BRF_MESH_H
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

#include "core/io/file_stream_reader.h"
#include "core/mb.h"

#include "core/platform/vertex_array.h"

namespace brf
{
	struct Face
	{
		explicit Face() noexcept = default;
		explicit Face(const int i, const int j, const int k) noexcept
			: i(i)
			, j(j)
			, k(k)
		{}

		bool load(FileStreamReader& stream);

		inline Face& operator+(const int scalar)
		{
			i = i + scalar;
			j = j + scalar;
			k = k + scalar;
			return *this;
		}

		inline void flip()
		{
			int temp = i;
			i = j;
			j = temp;
		}

		int i, j, k;
	};

	struct Vertex
	{
		bool load(FileStreamReader& stream);

		int m_index;
		uint32_t m_color;
		glm::vec3 m_normal;
		glm::vec3 m_origin;
		glm::vec3 m_tangent;
		uint8_t m_tbn;
		glm::vec2 m_texture_a, m_texture_b;

		// skeletal animation system
		glm::vec4 m_bone_weight;
		glm::vec4 m_bone_index;
	};

	struct Frame
	{
		bool load(FileStreamReader& stream);

		const glm::vec3 min() const;
		const glm::vec3 max() const;

		int find_closest_point(const glm::vec3& point, const float max_distance) const;

		int m_time;
		std::vector<glm::vec3> m_origins;
		std::vector<glm::vec3> m_normals;
	};

	struct Skinning 
	{
		int try_get_empty() const;
		int get_smallest_index_width() const; // index with the smallest weight
		void normalize();

		void add(const int index, const float weight);

		glm::vec4 cast_bone_index() const;
		glm::vec4 cast_bone_weight() const;

		int m_bone_index[4] { -1, -1, -1, -1 };
		float m_bone_weight[4] { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	class Mesh
	{
	public:
		bool load(FileStreamReader& stream);
		void precache(int flags);

		void apply_for_batching(std::vector<Vertex>& batch_vertices,
			std::vector<uint32_t>& batch_indices,
			const glm::vec3& origin,
			const glm::vec3& rotation,
			const glm::vec3& scale);

		int get_max_bone() const;
		const std::string& get_name() const;
		const std::string& get_material() const;
		const std::vector<Frame>& get_frames() const;
		const std::vector<uint32_t>& get_indices() const;
		const std::vector<Vertex>& get_vertices() const;

		Unique<mbcore::VertexArray> m_vertex_array;
	private:
		int m_max_bone_index;
		std::string m_name;
		std::string m_material;
		std::vector<Frame> m_frames;
		std::vector<uint32_t> m_indices;
		std::vector<Vertex> m_vertices;
		std::vector<Skinning> m_skinning;
	};
}

#endif // !_BRF_MESH_H
