#include "dds.h"

#include "utils/ddspp.h"

#include "core/platform/opengl/opengl.h"
#include "core/graphics/renderer.h"

#include "file_descriptor.h"

using namespace mbcore;

static uint32_t get_format_from_descriptor(const ddspp::Descriptor& descriptor) noexcept
{
	if (Renderer::API == Renderer::OpenGL)
	{
		switch (descriptor.format)
		{
		case ddspp::BC1_UNORM:
			return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		case ddspp::BC2_UNORM:
			return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		case ddspp::BC3_UNORM:
			return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}
	}

	return descriptor.format;
}

bool DDSTexture::load(const std::string& path, TextureProperties& properties)
{
	FileDescriptor file = FileDescriptor::open(path);
	if (file.m_info == nullptr)
		return false;

	ddspp::Descriptor descriptor {};
	ddspp::Result result = ddspp::decode_header(file.m_info.get(), descriptor);

	if (result == ddspp::Error)
		return false;

	properties.m_width = descriptor.width;
	properties.m_height = descriptor.height;
	properties.m_size = descriptor.depthPitch;
	properties.m_format = get_format_from_descriptor(descriptor);
	properties.m_start = file.m_info.get() + descriptor.headerSize;
	properties.m_texture = std::move(file.m_info);
	properties.m_is_compressed = true;

	return true;
}
