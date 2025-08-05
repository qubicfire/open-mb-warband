#include "opengl.h"

#include "opengl_texture.h"

OpenGLTexture2D::OpenGLTexture2D(const mbcore::TextureProperties& properties)
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

void OpenGLTexture2D::initialize(const mbcore::TextureProperties& properties)
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

OpenGLTexture2DArray::OpenGLTexture2DArray(const mbcore::TextureArrayProperties& properties)
{
	initialize(properties);
}

OpenGLTexture2DArray::~OpenGLTexture2DArray()
{
	glDeleteTextures(1, &m_id);
}

void OpenGLTexture2DArray::bind() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
}

void OpenGLTexture2DArray::initialize(const mbcore::TextureArrayProperties& properties)
{
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
}
