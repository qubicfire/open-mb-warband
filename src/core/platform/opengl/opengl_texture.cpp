#include "opengl.h"

#include "opengl_texture.h"

#include "core/io/dds.h"
#include "core/io/png.h"

using namespace mbcore;

OpenGLTexture2D::OpenGLTexture2D(const TextureProperties& properties)
{
	initialize(properties);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &m_id);
}

void OpenGLTexture2D::bind(const uint32_t slot) const
{
	glBindTextureUnit(slot, m_id);
}

void OpenGLTexture2D::initialize(const TextureProperties& properties)
{
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (properties.m_is_compressed)
	{
		glCompressedTexImage2D(GL_TEXTURE_2D,
			0,
			properties.m_format,
			properties.m_width,
			properties.m_height,
			0,
			properties.m_size,
			properties.m_start);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D,
			0,
			properties.m_format,
			properties.m_width,
			properties.m_height,
			0,
			properties.m_format,
			GL_UNSIGNED_BYTE,
			properties.m_start);
	}

	glGenerateMipmap(GL_TEXTURE_2D);
}

OpenGLTexture2DArray::OpenGLTexture2DArray(const TextureArrayProperties& properties)
	: m_layer(0)
{
	initialize(properties);
}

OpenGLTexture2DArray::~OpenGLTexture2DArray()
{
	glDeleteTextures(1, &m_id);
}

void OpenGLTexture2DArray::bind() const
{
	glBindTextureUnit(0, m_id);
}

void OpenGLTexture2DArray::generate_mipmaps() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void OpenGLTexture2DArray::add_texture(const std::string& path, 
	const Texture2D::Type type)
{
	TextureProperties properties {};
	bool is_loaded = false;

	switch (type)
	{
	case Texture2D::DDS:
		is_loaded = DDSTexture::load(path, properties);
		break;
	case Texture2D::PNG:
		is_loaded = PNGTexture::load(path, properties);
		break;
	}

	if (!is_loaded)
		PNGTexture::load("test/missing_texture.png", properties);

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);

	if (properties.m_is_compressed)
	{
		glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,
			0,
			0,
			m_layer++,
			properties.m_width,
			properties.m_height,
			1,
			properties.m_format,
			properties.m_size,
			properties.m_start);
	}
	else
	{
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,
			0,
			0, 
			m_layer++, // mip, x, y, layer
			properties.m_width,
			properties.m_height,
			1,
			properties.m_format,
			GL_UNSIGNED_BYTE,
			properties.m_start);
	}
}

void OpenGLTexture2DArray::initialize(const mbcore::TextureArrayProperties& properties)
{
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY,
		1,
		GL_RGBA8,
		properties.m_width,
		properties.m_height,
		properties.m_layers);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
