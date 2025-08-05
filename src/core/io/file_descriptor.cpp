#include "utils/log.h"

#include "file_descriptor.h"

FileDescriptor FileDescriptor::open(const std::string& path)
{
	FileDescriptor file {};
	std::ifstream file_stream(path, std::ios::binary | std::ios::in | std::ios::ate);

	if (!file_stream)
	{
		log_alert("Failed to load file \'%s\'", path.c_str());
		return file;
	}

	const size_t length = file_stream.tellg();
	file_stream.seekg(0, std::ios::beg);

	file.m_info = create_unique<uint8_t[]>(length);
	file.m_length = static_cast<uint32_t>(length);
	file_stream.read(reinterpret_cast<char*>(file.m_info.get()), length);

	return file;
}
