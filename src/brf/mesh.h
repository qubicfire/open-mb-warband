#ifndef _BRF_MESH_H
#define _BRF_MESH_H
#include "utils/mb_small_array.h"
#include "utils/mb_string.h"

#include "core/io/file_stream_reader.h"
#include "core/mb.h"

#include "core/platform/vertex_array.h"

struct AABB
{
	glm::vec3 m_min;
	glm::vec3 m_max;
};

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

		float m_time;

		// used for frustum culling
		glm::vec3 m_min;
		glm::vec3 m_max;

		mb_small_array<glm::vec3> m_origins;
		mb_small_array<glm::vec3> m_normals;
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

	struct MeshAttribute
	{
		VertexType m_type;
		int m_stride;
		const void* m_pointer;
		bool m_normalized = false;
	};

	class Mesh
	{
	public:
		bool load(FileStreamReader& stream);
		void precache(AABB& aabb, int flags);

		void apply_for_batching(std::vector<Vertex>& batch_vertices,
			std::vector<uint32_t>& batch_indices,
			const glm::vec3& origin,
			const glm::vec3& rotation,
			const glm::vec3& scale);

		int get_bone() const;
		const std::string& get_name() const;
		const std::string& get_material() const;
		const mb_small_array<Frame>& get_frames() const;
		const mb_small_array<uint32_t>& get_indices() const;
		const mb_small_array<Vertex>& get_vertices() const;

		mb_unique<mbcore::VertexArray> m_vertex_array;
	private:
		int m_bone;
		std::string m_name;
		std::string m_material;
		mb_small_array<Frame> m_frames;
		mb_small_array<uint32_t> m_indices;
		mb_small_array<Vertex> m_vertices;
		mb_small_array<Skinning> m_skinning;
	};

	struct MeshBuilder
	{
		template <class _Tx, class... _Args>
		static inline Mesh* create(const std::vector<_Tx>& vertices,
			int flags,
			_Args&&... args)
		{
			return create_ex(vertices,
				flags,
				std::index_sequence_for<_Args...>(),
				std::forward<_Args>(args)...);
		}

		template <class _Tx, class... _Args>
		static inline Mesh* create(const std::vector<_Tx>& vertices,
			const std::vector<uint32_t>& indices,
			int flags,
			_Args&&... args)
		{
			return create_ex(vertices,
				indices,
				flags,
				std::index_sequence_for<_Args...>(),
				std::forward<_Args>(args)...);
		}
	private:
		template <class _Tx, class... _Args, size_t... _Indices>
		static inline Mesh* create_ex(const std::vector<_Tx>& vertices,
			int flags,
			std::index_sequence<_Indices...>,
			_Args&&... args)
		{
			using namespace mbcore;

			mb_unique<VertexArray> vertex_array = VertexArray::create(VertexFlags::Triangles);
			mb_unique<VertexBuffer> vertex_buffer = VertexBuffer::create(vertices, flags);

			(vertex_array->link(_Indices, args.m_type, args.m_stride, args.m_pointer, args.m_normalized), ...);

			vertex_array->set_vertex_buffer(vertex_buffer);

			vertex_array->unbind();

			brf::Mesh* mesh = new Mesh();
			mesh->m_vertex_array = std::move(vertex_array);
			mesh_safety_storage(mesh);

			return mesh;
		}
		template <class _Tx, class... _Args, size_t... _Indices>
		static inline Mesh* create_ex(const std::vector<_Tx>& vertices,
			const std::vector<uint32_t>& indices,
			int flags,
			std::index_sequence<_Indices...>,
			_Args&&... args)
		{
			using namespace mbcore;

			mb_unique<VertexArray> vertex_array = VertexArray::create(VertexFlags::Indexes);
			mb_unique<VertexBuffer> vertex_buffer = VertexBuffer::create(vertices, flags);

			(vertex_array->link(_Indices, args.m_type, args.m_stride, args.m_pointer, args.m_normalized), ...);

			mb_unique<IndexBuffer> index_buffer = IndexBuffer::create(indices);

			vertex_array->set_vertex_buffer(vertex_buffer);
			vertex_array->set_index_buffer(index_buffer);

			vertex_array->unbind();

			brf::Mesh* mesh = new Mesh();
			mesh->m_vertex_array = std::move(vertex_array);
			mesh_safety_storage(mesh);

			return mesh;
		}
		static void mesh_safety_storage(Mesh* mesh);
	};
}

#endif // !_BRF_MESH_H
