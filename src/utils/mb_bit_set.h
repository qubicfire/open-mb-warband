#ifndef _MB_BIT_SET_H
#define _MB_BIT_SET_H
#include "mb_config.h"

template <class _Tx, class _Dx = uint32_t>
class mb_bit_set final
{
public:
	constexpr MB_INLINE mb_bit_set() MB_NOEXCEPT = default;

	MB_INLINE mb_bit_set(_Tx&& flag) MB_NOEXCEPT
		: m_bits(flag)
	{ }

	MB_INLINE mb_bit_set(_Dx&& flag) MB_NOEXCEPT
		: m_bits(static_cast<_Tx>(flag))
	{ }

	MB_INLINE ~mb_bit_set() = default;

	MB_INLINE void set(_Tx&& flag) MB_NOEXCEPT
	{
		m_bits |= static_cast<_Dx>(flag);
	}

	MB_INLINE void set_bits(const _Tx& flags) MB_NOEXCEPT
	{
		m_bits = static_cast<_Dx>(flags);
	}

	MB_INLINE void clear_bit(_Tx&& flag) MB_NOEXCEPT
	{
		m_bits &= ~(static_cast<_Dx>(flag));
	}

	MB_INLINE const bool is_bit_set(_Tx&& flag) MB_NOEXCEPT
	{
		return m_bits & static_cast<_Dx>(flag);
	}

	MB_INLINE void change_bit(_Tx&& flag, _Tx&& change) MB_NOEXCEPT
	{
		m_bits &= ~(static_cast<_Dx>(flag));
		m_bits |= static_cast<_Dx>(change);
	}

	MB_INLINE const bool try_clear_bit(_Tx&& flag) MB_NOEXCEPT
	{
		if ((m_bits & static_cast<_Dx>(flag)))
		{
			m_bits &= ~(static_cast<_Dx>(flag));
			return true;
		}

		return false;
	}

	MB_INLINE const bool try_change_bit(_Tx&& flag, _Tx&& change) MB_NOEXCEPT
	{
		if ((m_bits & static_cast<_Dx>(flag)))
		{
			m_bits &= ~(static_cast<_Dx>(flag));
			m_bits |= static_cast<_Dx>(change);
			return true;
		}

		return false;
	}

	MB_INLINE void clear_bits() MB_NOEXCEPT
	{
		m_bits = 0;
	}

	MB_INLINE _Dx get() const MB_NOEXCEPT
	{
		return m_bits; 
	}
private:
	_Dx m_bits;
};

#endif // !_MB_BIT_SET_H
