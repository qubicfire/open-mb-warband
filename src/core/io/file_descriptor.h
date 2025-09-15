#ifndef _FILE_DESCRIPTOR_H
#define _FILE_DESCRIPTOR_H
#include <fstream>

#include "core/mb_type_traits.h"

struct FileDescriptor
{
	static FileDescriptor open(const std::string& path);

	mb_unique<uint8_t[]> m_info;
	uint32_t m_length;
};

#endif // _FILE_DESCRIPTOR_H
