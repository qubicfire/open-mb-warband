#ifndef _MB_SMALL_ARRAY_H
#define _MB_SMALL_ARRAY_H
#include <cstdint>

template <class _Tx>
struct mb_small_array final
{
	struct mb_default_allocator final
	{ 
		static _Tx* allocate(const size_t size) noexcept
		{
			return new _Tx[size];
		}
	};

	class iterator final
	{
	public:
		explicit inline iterator(_Tx* pointer) noexcept
			: m_pointer(pointer)
		{ }

		inline iterator operator++() noexcept
		{
			m_pointer++;
			return *this;
		}
		inline iterator operator++(int) noexcept
		{
			iterator self = *this;
			m_pointer++;
			return self;
		}

		inline _Tx& operator*() noexcept 
		{
			return *m_pointer;
		}
		inline _Tx* operator->() noexcept
		{
			return m_pointer;
		}
		inline bool operator==(const iterator& other) noexcept
		{
			return m_pointer == other.m_pointer; 
		}
		inline bool operator!=(const iterator& other) noexcept
		{
			return m_pointer != other.m_pointer; 
		}
	private:
		_Tx* m_pointer;
	};

	using const_iterator = iterator;

	inline mb_small_array() noexcept = default;
	inline explicit mb_small_array(const size_t size)
		: m_array(mb_default_allocator::allocate(size))
		, m_size(size)
	{ }

	inline ~mb_small_array() { delete[] m_array; }

	inline _Tx& operator[](const size_t position) noexcept
	{
		return m_array[position];
	}
	inline const _Tx& operator[](const size_t position) const noexcept
	{
		return m_array[position];
	}

	[[nodiscard]] inline iterator begin() noexcept
	{
		return iterator(m_array);
	}
	[[nodiscard]] inline const_iterator begin() const noexcept
	{
		return iterator(m_array);
	}
	[[nodiscard]] inline iterator end() noexcept
	{
		return iterator(m_array + m_size);
	}
	[[nodiscard]] inline const_iterator end() const noexcept
	{
		return iterator(m_array + m_size);
	}

	_Tx* m_array;
	size_t m_size;
};

#endif // !_MB_SMALL_ARRAY_H
