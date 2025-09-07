#ifndef _BATCH3D_H
#define _BATCH3D_H
#include "core/objects/object.h"
#include "core/platform/vertex_array.h"
#include "core/graphics/renderer.h"
#include "brf/mesh.h"

#include "utils/tsl/robin_map.h"

struct Batch3DContext
{
	std::vector<brf::Vertex> m_vertices;
	std::vector<uint32_t> m_indices;

	Unique<mbcore::Texture2D> m_texture;
	Unique<mbcore::VertexArray> m_array;
};

class Batch3D final
{
public:
	template <class _Tx, 
		std::enable_if_t<std::is_base_of_v<Object, _Tx>, int> = 0>
	static void draw_mesh(const glm::vec3& origin,
		const glm::vec3& rotation,
		const glm::vec3& scale,
		brf::Mesh* mesh)
	{
		using namespace mbcore;

		Batch3DContext& context = m_contexts[Object::get_static_object_base_id<_Tx>()];
		mesh->apply_for_batching(origin, rotation, scale, context.m_vertices, context.m_indices);

		Unique<VertexArray> vertex_array = VertexArray::create();
		Unique<VertexBuffer> vertex_buffer = VertexBuffer::create(context.m_vertices, true);

		vertex_array->link(0, VertexType::Float3, cast_offset(brf::Vertex, m_origin));
		vertex_array->link(1, VertexType::Float3, cast_offset(brf::Vertex, m_normal));
		vertex_array->link(2, VertexType::Float2, cast_offset(brf::Vertex, m_texture_a));
		vertex_array->link(3, VertexType::Uint, cast_offset(brf::Vertex, m_color));

		Unique<IndexBuffer> index_buffer = IndexBuffer::create(context.m_indices);

		vertex_array->set_vertex_buffer(vertex_buffer);
		vertex_array->set_index_buffer(index_buffer);

		vertex_array->unbind();
		context.m_array = std::move(vertex_array);
	}

	static void draw_all()
	{
		for (const auto& [id, context] : m_contexts)
			Renderer::draw_vertex_array(context.m_array);

		m_contexts.clear();
	}
private:
	static inline mb_hash_map<uint32_t, Batch3DContext> m_contexts {};
};

#endif // !_BATCH3D_H
