#ifndef _MB_SPAN_H
#define _MB_SPAN_H
#include "mb_config.h"

template <class _Tx>
struct mb_span final
{
	explicit MB_INLINE mb_span() MB_NOEXCEPT
		: m_data(nullptr), m_size(0)
	{ }

	explicit MB_INLINE mb_span(_Tx* data, const size_t size) MB_NOEXCEPT
		: m_data(data), m_size(size)
	{ }

	MB_INLINE operator _Tx*() MB_NOEXCEPT
	{
		return m_data;
	}

	_Tx* m_data;
	size_t m_size;
};

#endif // !_MB_SPAN_H
