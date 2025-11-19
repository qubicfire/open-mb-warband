#ifndef _DDS_TEXTURE_H
#define _DDS_TEXTURE_H
#include "core/platform/texture2d.h"

struct DDSTexture
{
	static bool load(const std::string& path, mbcore::TextureProperties& properties);
};

#endif // !_DDS_TEXTURE_H
