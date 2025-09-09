#ifndef _MB_STRING_H
#define _MB_STRING_H
#include "memory/mb_allocator.h"
#include "mb_span.h"

template <class _Tx>
struct mb_basic_string final
{
	using allocator = mb_default_allocator<_Tx[]>;
	using char_traits = mb_char_traits<_Tx, allocator>;

	explicit MB_INLINE mb_basic_string() MB_NOEXCEPT
		: m_size(0), m_capacity(0), m_chars(nullptr)
	{ }

	template <size_t _Size>
	explicit MB_INLINE mb_basic_string(const _Tx (&chars)[_Size]) MB_NOEXCEPT
		: m_size(_Size)
		, m_capacity(_Size)
		, m_chars(char_traits::construct(chars, _Size))
	{ }

	explicit MB_INLINE mb_basic_string(const mb_basic_string<_Tx>& other) MB_NOEXCEPT
		: m_size(other.m_size)
		, m_capacity(other.m_capacity)
		, m_chars(char_traits::construct(other.m_chars, other.m_size))
	{ }

	MB_INLINE ~mb_basic_string()
	{ 
		allocator::deallocate(m_chars);
	}

	MB_INLINE void reserve(const size_t new_capacity)
	{
		if (m_size > 0)
		{
			m_chars = char_traits::assign(m_chars, m_size, new_capacity);
		}
		else
		{
			m_chars = allocator::allocate(new_capacity + 1);
			m_capacity = new_capacity;

			m_chars[new_capacity] = 0;
		}
	}

	MB_INLINE void resize(const size_t new_size)
	{
		m_chars = char_traits::assign(m_chars, m_size, new_size);
	}

	MB_INLINE void push_back(const _Tx chars)
	{
		const size_t next_index = m_size + 1;

		if (next_index > m_capacity)
		{
			m_capacity = m_capacity << 1;
			m_chars = char_traits::assign(m_chars, m_size, m_capacity);
		}

		m_chars[m_size] = chars;
		m_size = next_index;
	}

	MB_INLINE mb_span<_Tx> sub_span(const size_t begin, const size_t end) MB_NOEXCEPT
	{
		return mb_span<_Tx>(m_chars + begin, end - begin);
	}

	MB_INLINE mb_basic_string& operator=(const mb_basic_string<_Tx>& other)
	{
		m_chars = char_traits::assign(m_chars, other.m_chars, other.m_size);

		m_size = other.m_size;
		m_capacity = other.m_capacity;

		return *this;
	}

	MB_INLINE mb_basic_string operator+(const mb_basic_string<_Tx>& other)
	{
		mb_basic_string basic_string;
		const size_t new_size = m_size + other.m_size;
		basic_string.m_chars = allocator::allocate(new_size + 1);
		basic_string.m_chars[new_size] = 0;
		basic_string.m_size = new_size;
		basic_string.m_capacity = new_size;

		std::memcpy(basic_string.m_chars, m_chars, m_size);
		std::memcpy(basic_string.m_chars, other.m_chars, other.m_size);

		return basic_string;
	}

	MB_INLINE bool operator==(const mb_basic_string<_Tx>& other) MB_NOEXCEPT
	{
		return char_traits::compare(m_chars, m_size, other.m_chars, other.m_size);
	}
	MB_INLINE bool operator!=(const mb_basic_string<_Tx>& other) MB_NOEXCEPT
	{
		return !char_traits::compare(m_chars, m_size, other.m_chars, other.m_size);
	}

	MB_INLINE _Tx& operator[](const size_t index)
	{
		return m_chars[index];
	}
	MB_INLINE const _Tx& operator[](const size_t index) const
	{
		return m_chars[index];
	}

	MB_INLINE operator _Tx*() MB_NOEXCEPT
	{
		return m_chars;
	}

	size_t m_size;
	size_t m_capacity;

	_Tx* m_chars;
};

using mb_string = mb_basic_string<char>;

#endif // !_MB_STRING_H
