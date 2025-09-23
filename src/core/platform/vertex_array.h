#ifndef _VERTEX_ARRAY_H
#define _VERTEX_ARRAY_H
#include "core/mb_type_traits.h"
#include "buffer.h"

enum class VertexType : uint8_t
{
	Float,
	Float2,
	Float3,
	Float4,
	UShort,
	Uint,
	Uint4,
	Int,
	Int4,
	Count
};

namespace mbcore
{
	enum RendererType : int8_t
	{
		Triangles,
		Indexes,
		Instancing,
		Indirect,
	};

	struct VertexArray
	{
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void link(uint32_t index, 
			VertexType type,
			int stride,
			const void* pointer,
			bool normalized = false) const = 0;

		virtual void initialize() = 0;

		void set_vertex_buffer(mb_unique<Buffer>& buffer);
		void set_index_buffer(mb_unique<Buffer>& buffer);

		const RendererType get_flags() const;
		const uint32_t get_id() const;
		Buffer* get_vertex_buffer() const;
		Buffer* get_index_buffer() const;

		static mb_unique<VertexArray> create(int flags = RendererType::Indexes);
	protected:
		RendererType m_flags;
		mb_unique<Buffer> m_vertex_buffer;
		mb_unique<Buffer> m_index_buffer;
		uint32_t m_id;
	};
}

#endif // !_VERTEX_ARRAY_H
