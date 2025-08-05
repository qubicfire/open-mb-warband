#ifndef _BRF_TEXTURE_H
#define _BRF_TEXTURE_H
#include "core/mb.h"
#include "core/io/file_stream_reader.h"

namespace brf
{
	class Texture
	{
	public:
		bool load(FileStreamReader& stream);
	private:
		std::string m_name;
		uint32_t m_flags;
	};
}

#endif // !_BRF_TEXTURE_H
