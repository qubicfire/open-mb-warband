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
	struct VertexArray
	{
		enum Types : int8_t
		{
			Triangles,
			Indexes,
			Instancing,
			Indirect,
		};

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void link(uint32_t index, 
			VertexType type,
			int stride,
			const void* pointer,
			bool normalized = false) const = 0;

		virtual void initialize() = 0;

		void set_vertex_buffer(mb_unique<Buffer>& buffer, int index = 0);
		void set_index_buffer(mb_unique<Buffer>& buffer);

		const Types get_flags() const;
		const uint32_t get_id() const;
		Buffer* get_vertex_buffer(int index = 0) const;
		mb_small_array<mb_unique<Buffer>>& get_vertex_buffers();
		Buffer* get_index_buffer() const;

		static mb_unique<VertexArray> create(int buffers, 
			const Types flags = Types::Indexes);
	protected:
		mb_small_array<mb_unique<Buffer>> m_vertex_buffers;
		mb_unique<Buffer> m_index_buffer;

		uint32_t m_id;
		Types m_flags;
	};
}

#endif // !_VERTEX_ARRAY_H
