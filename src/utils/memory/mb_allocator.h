#ifndef _MB_ALLOCATOR_H
#define _MB_ALLOCATOR_H
#include <cstdlib>

template <class _Tx, class _Allocator>
struct mb_char_traits final
{
	static _Tx* assign(_Tx* source,
		const size_t old_size,
		const size_t new_size)
	{
		_Tx* new_chars = _Allocator::allocate(new_size + 1);

		std::memcpy(new_chars, source, old_size);

		new_chars[new_size] = 0;

		_Allocator::deallocate(source);

		return new_chars;
	}

	static _Tx* assign(_Tx* destination,
		const _Tx* source,
		const size_t new_size)
	{
		_Tx* new_chars = _Allocator::allocate(new_size + 1);

		std::memcpy(new_chars, source, new_size);

		new_chars[new_size] = 0;

		_Allocator::deallocate(destination);

		return new_chars;
	}

	static _Tx* construct(const _Tx* source, const size_t size)
	{
		_Tx* new_chars = _Allocator::allocate(size + 1);

		std::memcpy(new_chars, source, size);

		new_chars[size] = 0;

		return new_chars;
	}

	static bool compare(const _Tx* source_a, 
		const size_t size_a, 
		const _Tx* source_b,
		const size_t size_b)
	{
		return size_a == size_b &&
			*reinterpret_cast<long*>(const_cast<_Tx*>(source_a)) == *reinterpret_cast<long*>(const_cast<_Tx*>(source_b));
	}
};

template <class _Tx>
struct mb_default_allocator final
{
	static _Tx* allocate()
	{
		return new _Tx();
	}

	static void deallocate(_Tx* block)
	{
		delete block;
	}
};

template <class _Tx>
struct mb_default_allocator<_Tx[]> final
{
	static _Tx* allocate(const size_t size)
	{
		return new _Tx[size];
	}

	static void deallocate(_Tx* block)
	{
		delete[] block;
	}
};

#endif // !_MB_ALLOCATOR_H
