#ifndef _FLAGSTORAGE_H
#define _FLAGSTORAGE_H

template <class _Tx, class _Dx = unsigned int>
class FlagStorage
{
public:
	constexpr inline FlagStorage() noexcept = default;
	inline FlagStorage(_Tx&& flag) noexcept
		: m_storage(flag)
	{ }

	~FlagStorage() = default;

	inline void set_flag(_Tx&& flag) noexcept
	{
		m_storage |= static_cast<_Dx>(flag);
	}

	inline void clear_flag(_Tx&& flag) noexcept
	{
		m_storage &= ~(static_cast<_Dx>(flag));
	}

	inline const bool is_flag_set(_Tx&& flag) noexcept
	{
		return m_storage & static_cast<_Dx>(flag);
	}

	inline void change_flag(_Tx&& flag, _Tx&& change) noexcept
	{
		m_storage &= ~(static_cast<_Dx>(flag));
		m_storage |= static_cast<_Dx>(change);
	}

	inline const bool try_clear_flag(_Tx&& flag) noexcept
	{
		if ((m_storage & static_cast<_Dx>(flag)))
		{
			m_storage &= ~(static_cast<_Dx>(flag));
			return true;
		}

		return false;
	}

	inline const bool try_change_flag(_Tx&& flag, _Tx&& change) noexcept
	{
		if ((m_storage & static_cast<_Dx>(flag)))
		{
			m_storage &= ~(static_cast<_Dx>(flag));
			m_storage |= static_cast<_Dx>(change);
			return true;
		}

		return false;
	}

	inline void clear() noexcept
	{
		m_storage = 0;
	}

	inline _Dx get_storage() const noexcept
	{
		return m_storage; 
	}
private:
	_Dx m_storage;
};

#endif // !_FLAGSTORAGE_H
