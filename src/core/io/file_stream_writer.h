#ifndef _FILE_SAVE_STREAM_H
#define _FILE_SAVE_STREAM_H
#include "core/mb.h"

class FileStreamWriter
{
public:
	explicit FileStreamWriter() noexcept
		: m_buffer(nullptr)
		, m_size(0)
		, m_capacity(0)
	{}

	~FileStreamWriter();

	template <class _T>
	inline void try_resize_buffer(const uint32_t size = sizeof(_T))
	{
		if (m_size + size > m_capacity)
		{
			m_capacity = (m_size + size) * 2;

			uint8_t* previous_buffer = m_buffer;
			m_buffer = static_cast<uint8_t*>(
				std::realloc(m_buffer, m_capacity)
			);

			if (m_buffer == nullptr)
				std::free(previous_buffer);
		}
	}

	template <class _Tx>
	inline void write(const _Tx value)
	{
		constexpr uint32_t size = sizeof(_Tx);

		try_resize_buffer<_Tx>();
		std::memcpy(m_buffer + m_size, &value, size);

		m_size = m_size + size;
	}

	void write_int(const int value);
	void write_float(const float value);
	void write_double(const double value);
	void write_string(const std::string& value);

	const uint32_t get_size() const noexcept;
	const uint8_t* get_buffer() const noexcept;
private:
	template <class _Tx>
	inline void assign_buffer(const uint32_t size, const void* source)
	{
		try_resize_buffer<_Tx>(size);
		std::memcpy(m_buffer + m_size, source, size);

		m_size = m_size + size;
	}
private:
	uint8_t* m_buffer;
	uint32_t m_size;
	uint32_t m_capacity;
};

#endif // !_FILE_SAVE_STREAM_H
