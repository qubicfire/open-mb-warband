#ifndef _PNG_H
#define _PNG_H
#include "core/mb.h"
#include "core/platform/texture2d.h"

struct PNGTexture
{
	static bool load(const std::string& path, mbcore::TextureProperties& properties);
};

#endif // !_PNG_H
