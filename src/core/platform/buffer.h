#ifndef _BUFFER_H
#define _BUFFER_H
#include "core/mb.h"
#include "core/mb_type_traits.h"
#include "utils/mb_small_array.h"

namespace mbcore
{
	struct Buffer
	{
		mb_enum_friend_class(Types)
		{
			Array = (1 << 0),
			Element = (1 << 1),
			Indirect = (1 << 2),
			Storage = (1 << 3),
			
			Dynamic = (1 << 4),
			Static = (1 << 5),
			Persistent = (1 << 6),
		};

		virtual void bind() const = 0;
		virtual void sub_data(const size_t offset, const size_t size, const void* data) = 0;
		virtual void get_sub_data(const size_t offset, const size_t size, void* data) = 0;
		virtual void* map_buffer_range() const = 0;

		template <class _Tx>
		inline _Tx* map_buffer_range() const
		{
			return static_cast<_Tx*>(
				map_buffer_range()
			);
		}

		static mb_unique<Buffer> create(const void* vertices, 
			const size_t count,
			const size_t size,
			const Buffer::Types flags = Buffer::Types::Static);
		
		template <class _Tx>
		static inline mb_unique<Buffer> create(const std::vector<_Tx>& vertices,
			const Buffer::Types flags = Buffer::Types::Static)
		{
			return create(vertices.data(),
				vertices.size(), 
				sizeof(_Tx),
				flags);
		}

		template <class _Tx>
		static inline mb_unique<Buffer> create(const mb_small_array<_Tx>& vertices,
			const Buffer::Types flags = Buffer::Types::Static)
		{
			return create(vertices.m_array,
				vertices.m_size,
				sizeof(_Tx),
				flags);
		}

		template <class _Tx, size_t _Size>
		static inline mb_unique<Buffer> create(const std::array<_Tx, _Size>& vertices,
			const Buffer::Types flags = Buffer::Types::Static)
		{
			return create(vertices.data(),
				vertices.size(),
				sizeof(_Tx),
				flags);
		}

		uint32_t m_id;
		uint32_t m_type;
		size_t m_size;
		size_t m_count;
	protected:
		virtual void initialize(const void* vertices,
			const size_t count,
			const size_t size,
			const Buffer::Types flags) = 0;
	};
}

#endif // !_BUFFER_H
