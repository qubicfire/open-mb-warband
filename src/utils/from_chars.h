#ifndef _FROM_CHARS_H
#define _FROM_CHARS_H
#include "core/mb.h"
#include "fast_float/fast_float.h"

namespace utils
{
	template <class _Tx>
	inline _Tx from_chars(std::string_view start) noexcept
	{
		_Tx result {};
		auto error_code = fast_float::from_chars(start.data(), start.data() + start.size(), result);

		return result;
	}
}

#endif // !_FROM_CHARS_H
