#ifndef _MB_SMALL_ARRAY_H
#define _MB_SMALL_ARRAY_H
#include "memory/mb_allocator.h"
#include "mb_config.h"

template <class _Tx>
struct mb_small_array final
{
	class iterator final
	{
	public:
		explicit MB_INLINE iterator(_Tx* pointer) MB_NOEXCEPT
			: m_pointer(pointer)
		{ }

		MB_INLINE iterator operator++() MB_NOEXCEPT
		{
			m_pointer++;
			return *this;
		}
		MB_INLINE iterator operator++(int) MB_NOEXCEPT
		{
			iterator self = *this;
			m_pointer++;
			return self;
		}

		MB_INLINE _Tx& operator*() MB_NOEXCEPT
		{
			return *m_pointer;
		}
		MB_INLINE _Tx* operator->() MB_NOEXCEPT
		{
			return m_pointer;
		}
		MB_INLINE bool operator==(const iterator& other) MB_NOEXCEPT
		{
			return m_pointer == other.m_pointer; 
		}
		MB_INLINE bool operator!=(const iterator& other) MB_NOEXCEPT
		{
			return m_pointer != other.m_pointer; 
		}
	private:
		_Tx* m_pointer;
	};

	using const_iterator = iterator;
	using allocator = mb_default_allocator<_Tx[]>;

	explicit MB_INLINE mb_small_array() MB_NOEXCEPT
		: m_array(nullptr), m_size(0)
	{ }

	explicit MB_INLINE mb_small_array(const mb_small_array& array)
		: m_array(allocator::allocate(array.m_size))
		, m_size(array.m_size)
	{
		std::memcpy(m_array, array.m_array, sizeof(_Tx) * m_size);
	}

	MB_INLINE ~mb_small_array()
	{
		allocator::deallocate(m_array);
	}

	MB_INLINE void resize(const size_t new_size)
	{
		if (m_array)
		{
			_Tx* old_array = m_array;

			m_array = allocator::allocate(new_size);

			std::memcpy(m_array, old_array, sizeof(_Tx) * m_size);

			allocator::deallocate(old_array);
		}
		else
		{
			m_array = allocator::allocate(new_size);
		}

		m_size = new_size;
	}

	MB_INLINE _Tx& front() const MB_NOEXCEPT
	{
		return m_array[0];
	}

	MB_INLINE _Tx& back() const MB_NOEXCEPT
	{
		return m_array[m_size - 1];
	}

	MB_INLINE size_t size() const MB_NOEXCEPT
	{
		return m_size;
	}

	MB_INLINE bool empty() const MB_NOEXCEPT
	{
		return m_size == 0;
	}

	MB_INLINE mb_small_array& operator=(const mb_small_array<_Tx>& other)
	{
		if (m_array)
			allocator::deallocate(m_array);

		m_size = other.m_size;
		m_array = allocator::allocate(m_size);
		std::memcpy(m_array, other.m_array, sizeof(_Tx) * m_size);

		return *this;
	}

	MB_INLINE _Tx& operator[](const size_t index) MB_NOEXCEPT
	{
		return m_array[index];
	}
	MB_INLINE const _Tx& operator[](const size_t index) const MB_NOEXCEPT
	{
		return m_array[index];
	}

	MB_NODISCARD MB_INLINE iterator begin() MB_NOEXCEPT
	{
		return iterator(m_array);
	}
	MB_NODISCARD MB_INLINE const_iterator begin() const MB_NOEXCEPT
	{
		return iterator(m_array);
	}
	MB_NODISCARD MB_INLINE iterator end() MB_NOEXCEPT
	{
		return iterator(m_array + m_size);
	}
	MB_NODISCARD MB_INLINE const_iterator end() const MB_NOEXCEPT
	{
		return iterator(m_array + m_size);
	}

	_Tx* m_array;
	size_t m_size;
};

#endif // !_MB_SMALL_ARRAY_H
