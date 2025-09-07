#ifndef _BUFFER_H
#define _BUFFER_H
#include "core/mb.h"
#include "core/mb_type_traits.h"
#include "utils/mb_small_array.h"

namespace mbcore
{
	enum BufferFlags : int
	{
		Dynamic = (1 << 0),
		Static = (1 << 1),
		Persistent = (1 << 2),
	};

	struct VertexBuffer
	{
		virtual void bind() const = 0;
		virtual void* map_buffer_range() const = 0;

		template <class _Tx>
		inline _Tx* map_buffer_range() const
		{
			return static_cast<_Tx*>(
				map_buffer_range()
			);
		}

		static Unique<VertexBuffer> create(const void* vertices, 
			const size_t count,
			const size_t size,
			int flags = BufferFlags::Static);
		
		template <class _Tx>
		static inline Unique<VertexBuffer> create(const std::vector<_Tx>& vertices,
			int flags = BufferFlags::Static)
		{
			return create(vertices.data(),
				vertices.size(), 
				sizeof(_Tx),
				flags);
		}

		template <class _Tx>
		static inline Unique<VertexBuffer> create(const mb_small_array<_Tx>& vertices,
			int flags = BufferFlags::Static)
		{
			return create(vertices.m_array,
				vertices.m_size,
				sizeof(_Tx),
				flags);
		}

		template <class _Tx, size_t _Size>
		static inline Unique<VertexBuffer> create(const std::array<_Tx, _Size>& vertices,
			int flags = BufferFlags::Static)
		{
			return create(vertices.data(),
				vertices.size(),
				sizeof(_Tx),
				flags);
		}

		uint32_t m_id;
		size_t m_size;
		size_t m_count;
	protected:
		virtual void initialize(const void* vertices,
			const size_t count,
			const size_t size,
			int flags) = 0;
	};

	struct IndexBuffer
	{
		static Unique<IndexBuffer> create(const uint32_t* indices, const size_t size);

		static inline Unique<IndexBuffer> create(const std::vector<uint32_t>& indices)
		{
			return create(indices.data(), indices.size());
		}

		static inline Unique<IndexBuffer> create(const mb_small_array<uint32_t>& indices)
		{
			return create(indices.m_array, indices.m_size);
		}

		template <size_t _Size>
		static inline Unique<IndexBuffer> create(const std::array<uint32_t, _Size>& indices)
		{
			return create(indices.data(), indices.size());
		}

		uint32_t m_id;
		size_t m_count;
	protected:
		virtual void initialize(const uint32_t* indices, const size_t size) = 0;
	};
}

#endif // !_BUFFER_H
