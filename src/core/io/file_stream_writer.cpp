#include "file_stream_writer.h"

FileStreamWriter::~FileStreamWriter()
{
	std::free(m_buffer);
}

void FileStreamWriter::write_int(const int value)
{
	write(value);
}

void FileStreamWriter::write_float(const float value)
{
	write(value);
}

void FileStreamWriter::write_double(const double value)
{
	write(value);
}

void FileStreamWriter::write_string(const std::string& value)
{
	const uint32_t size = static_cast<uint32_t>(value.size() + 1);

	assign_buffer<std::string>(size, value.data() + '\0');
}

const uint32_t FileStreamWriter::get_size() const noexcept
{
	return m_size;
}

const uint8_t* FileStreamWriter::get_buffer() const noexcept
{
	return m_buffer;
}
