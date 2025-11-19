#ifndef _FILE_READ_STREAM_H
#define _FILE_READ_STREAM_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include "utils/from_chars.h"

class FileStreamReader
{
public:
	explicit FileStreamReader() noexcept;
	explicit FileStreamReader(const FileStreamReader& stream, bool is_async = true) noexcept;
	~FileStreamReader();

	size_t open(const std::string& path);

	template <class _Tx,
		std::enable_if_t<!std::is_same_v<_Tx, std::string_view>, int> = 0>
	inline _Tx read() noexcept
	{
		_Tx value;
		std::memcpy(&value, m_data + m_offset, sizeof(_Tx));
		m_offset = m_offset + sizeof(_Tx);

		// TODO: Implement little-endian
		if constexpr (std::is_same_v<_Tx, glm::vec3>)
		{
			float temp = value.y;
			value.y = value.z;
			value.z = -temp;
		}

		return value;
	}

	// Method that helps to prevent allocation a new string if thats no need
	template <class _Tx,
		std::enable_if_t<std::is_same_v<_Tx, std::string_view>, int> = 0>
	inline std::string_view read() noexcept
	{
		char temp = m_data[m_offset++];

		while (std::isspace(temp))
			temp = m_data[m_offset++];

		size_t start_offset = m_offset - 1;
		uint32_t length {};

		while (!std::isspace(temp))
		{
			length++;
			temp = m_data[m_offset++];
		}

		return std::string_view(reinterpret_cast<char*>(m_data + start_offset), length);
	}

	std::string read_until(char symbol_until = ' ', char symbol_skip = '\n') noexcept
	{
		std::string result {};
		char temp = m_data[m_offset++];

		while (std::isspace(temp))
			temp = m_data[m_offset++];

		while (temp != symbol_until)
		{
			result += temp;
			temp = m_data[m_offset++];
		}

		return result;
	}

	std::string read_with_length() noexcept
	{
		const uint32_t length = read<uint32_t>();
		std::string result {};
		result.reserve(length);

		for (uint32_t i = 0; i < length; i++)
			result.push_back(m_data[m_offset++]);

		return result;
	}

	std::string_view read_with_length_fast() noexcept
	{
		const uint32_t length = read<uint32_t>();
		std::string_view storage(reinterpret_cast<char*>(m_data + m_offset), length);

		m_offset = m_offset + length;
		return storage;
	}

	std::string read_or_default(const std::string& default_return) noexcept
	{
		const uint32_t length = read<uint32_t>();

		if (length >= 99 && length <= 0)
			return default_return;

		std::string result {};
		result.reserve(length);

		for (uint32_t i = 0; i < length; i++)
			result.push_back(m_data[m_offset++]);

		return result;
	}

	template <class _Tx>
	inline _Tx number_from_chars()
	{
		std::string_view v = read<std::string_view>();
		return utils::from_chars<_Tx>(v);
	}

	inline const uint8_t* get_data() const noexcept
	{
		return m_data;
		//return m_data.get();
	}
private:
	bool m_is_async;
	size_t m_offset;
	//Shared<uint8_t[]> m_data;
	uint8_t* m_data;
};

#endif // !_FILE_READ_STREAM_H
