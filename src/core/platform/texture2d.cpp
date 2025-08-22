#include "core/graphics/renderer.h"
#include "utils/assert.h"

#include "core/managers/assets.h"

#include "texture2d.h"
#include "opengl/opengl_texture.h"

#include "core/io/dds.h"
#include "core/io/png.h"

using namespace mbcore;

Unique<Texture2D> Texture2D::create_internal(const std::string& path, const Type type)
{
	TextureProperties properties {};
	bool is_loaded = false;

	switch (type)
	{
	case DDS:
		is_loaded = DDSTexture::load(path, properties);
		break;
	case PNG:
		is_loaded = PNGTexture::load(path, properties);
		break;
	}

	if (!is_loaded)
		PNGTexture::load("test/missing_texture.png", properties);

	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLTexture2D>(properties);
	}

	core_assert_immediatly("%s", "Unable to create a texture. Renderer API is invalid");
	return nullptr;
}

Texture2D* Texture2D::create(const std::string& path, const Type type)
{
	return g_assets->load_texture(path, type);
}

Unique<Texture2D> Texture2D::create_empty()
{
	TextureProperties properties { };

	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLTexture2D>(properties);
	}

	core_assert_immediatly("%s", "Unable to create a texture. Renderer API is invalid");
	return nullptr;
}

Unique<Texture2DArray> Texture2DArray::create(const TextureArrayProperties& properties)
{
	switch (Renderer::API)
	{
	case Renderer::OpenGL:
		return create_unique<OpenGLTexture2DArray>(properties);
	}

	core_assert_immediatly("%s", "Unable to create a texture array. Renderer API is invalid");
	return nullptr;
}
