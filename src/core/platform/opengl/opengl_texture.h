#ifndef _OPENGL_TEXTURE_H
#define _OPENGL_TEXTURE_H
#include "core/platform/texture2d.h"

struct OpenGLTexture2D : mbcore::Texture2D
{
	OpenGLTexture2D(const mbcore::TextureProperties& properties);
	~OpenGLTexture2D();

	void bind(const uint32_t slot = 0) const override;
protected:
	void initialize(const mbcore::TextureProperties& properties) override;
};

struct OpenGLTexture2DArray : mbcore::Texture2DArray
{
	OpenGLTexture2DArray(const mbcore::TextureArrayProperties& properties);
	~OpenGLTexture2DArray();

	void bind() const override;
protected:
	void initialize(const mbcore::TextureArrayProperties& properties) override;
};

#endif // !_OPENGL_TEXTURE_H