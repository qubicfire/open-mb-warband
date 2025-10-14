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

		glm::vec3 m_next_origin;
		glm::vec3 m_next_normal;
	};

	struct FrameVertex
	{
		glm::vec3 m_next_origin;
		glm::vec3 m_next_normal;
	};

	struct Frame
	{
		bool load(FileStreamReader& stream);

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

	//struct DrawElementsIndirectCommand
	//{
	//	uint32_t m_count;
	//	uint32_t m_instances;
	//	uint32_t m_index;
	//	int m_base_vertex;
	//	uint32_t m_base_instance;
	//};

	//struct MeshData
	//{
	//	glm::mat4 m_model;
	//	glm::vec4 m_bounding_aabb;
	//};

	class Mesh
	{
	public:
		Mesh() noexcept = default;
		Mesh(mb_unique<mbcore::VertexArray>& array) noexcept;

		bool load(FileStreamReader& stream, std::string& name);
		void precache(AABB& aabb, const mbcore::Buffer::Types flags);

		int get_bone() const;
		const std::string& get_material() const;
		const mb_small_array<Frame>& get_frames() const;
		const mb_small_array<Face>& get_faces() const;
		const mb_small_array<Vertex>& get_vertices() const;
		const mb_unique<mbcore::VertexArray>& get_vertex_array() const;
		Vertex* get_buffer() const;
	private:
		mb_unique<mbcore::VertexArray> m_vertex_array;
		Vertex* m_buffer;

		mb_small_array<Frame> m_frames;
		mb_small_array<Face> m_faces;
		mb_small_array<Vertex> m_vertices;
		mb_small_array<Skinning> m_skinning;

		int m_bone;
		std::string m_material;
	};

	struct MeshBuilder
	{
		template <class _Tx, class... _Args>
		static inline Mesh* create(const std::vector<_Tx>& vertices,
			const mbcore::Buffer::Types flags,
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
			const mbcore::Buffer::Types flags,
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
			const mbcore::Buffer::Types flags,
			std::index_sequence<_Indices...>,
			_Args&&... args)
		{
			using namespace mbcore;

			mb_unique<VertexArray> vertex_array = VertexArray::create(1, VertexArray::Triangles);
			mb_unique<Buffer> vertex_buffer = Buffer::create(vertices, Buffer::Types::Array | flags);

			(vertex_array->link(_Indices, args.m_type, args.m_stride, args.m_pointer, args.m_normalized), ...);

			vertex_array->set_vertex_buffer(vertex_buffer);

			vertex_array->unbind();

			Mesh* mesh = new Mesh(vertex_array);
			mesh_apply(mesh);

			return mesh;
		}
		template <class _Tx, class... _Args, size_t... _Indices>
		static inline Mesh* create_ex(const std::vector<_Tx>& vertices,
			const std::vector<uint32_t>& indices,
			const mbcore::Buffer::Types flags,
			std::index_sequence<_Indices...>,
			_Args&&... args)
		{
			using namespace mbcore;

			mb_unique<VertexArray> vertex_array = VertexArray::create(1, VertexArray::Indexes);
			mb_unique<Buffer> vertex_buffer = Buffer::create(vertices, Buffer::Types::Array | flags);

			(vertex_array->link(_Indices, args.m_type, args.m_stride, args.m_pointer, args.m_normalized), ...);

			mb_unique<Buffer> index_buffer = Buffer::create(vertices, Buffer::Types::Element);

			vertex_array->set_vertex_buffer(vertex_buffer);
			vertex_array->set_index_buffer(index_buffer);

			vertex_array->unbind();

			Mesh* mesh = new Mesh(vertex_array);
			mesh_apply(mesh);

			return mesh;
		}
		static void mesh_apply(Mesh* mesh);
	};
}

#endif // !_BRF_MESH_H
