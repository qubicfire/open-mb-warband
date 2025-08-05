#ifndef _BUFFER_H
#define _BUFFER_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

namespace mbcore
{
	struct VertexBuffer
	{
		static Unique<VertexBuffer> create(const void* vertices, 
			const uint32_t count, 
			const uint32_t size,
			bool is_static_draw = true);
		
		template <class _Tx>
		static inline Unique<VertexBuffer> create(const std::vector<_Tx>& vertices,
			bool is_static_draw = true)
		{
			return create(vertices.data(),
				static_cast<uint32_t>(vertices.size()), 
				sizeof(_Tx),
				is_static_draw);
		}

		template <class _Tx, size_t _Size>
		static inline Unique<VertexBuffer> create(const std::array<_Tx, _Size>& vertices,
			bool is_static_draw = true)
		{
			return create(vertices.data(),
				static_cast<uint32_t>(vertices.size()),
				sizeof(_Tx),
				is_static_draw);
		}

		uint32_t m_id;
	protected:
		virtual void initialize(const void* vertices,
			const uint32_t count,
			const uint32_t size,
			bool is_static_draw) = 0;
	};

	struct IndexBuffer
	{
		static Unique<IndexBuffer> create(const uint32_t* indices, const uint32_t size);

		static inline Unique<IndexBuffer> create(const std::vector<uint32_t>& indices)
		{
			return create(indices.data(), static_cast<uint32_t>(indices.size()));
		}

		template <size_t _Size>
		static inline Unique<IndexBuffer> create(const std::array<uint32_t, _Size>& indices)
		{
			return create(indices.data(), static_cast<uint32_t>(indices.size()));
		}

		uint32_t m_id;
		uint32_t m_count;
	protected:
		virtual void initialize(const uint32_t* indices, const uint32_t size) = 0;
	};
}

#endif // !_BUFFER_H
