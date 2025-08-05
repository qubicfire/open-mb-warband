#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

namespace mbcore
{
	struct TextureProperties
	{
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_size;
		uint32_t m_format;
		const uint8_t* m_start;
		Unique<uint8_t[]> m_texture;
		bool m_is_compressed;
	};

	struct TextureArrayProperties
	{
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_layers;
		uint32_t m_format;
	};

	struct Texture2D
	{
		enum Type : uint8_t
		{
			DDS,
			PNG,
			JPG
		};

		virtual void bind(const uint32_t slot = 0) const = 0;

		static Unique<Texture2D> create(const std::string& path, const Type type);
		[[deprecated]] static Unique<Texture2D> create_empty();

		uint32_t m_id;
	protected:
		virtual void initialize(const TextureProperties& properties) = 0;
	};

	struct Texture2DArray
	{
		virtual void bind() const = 0;

		static Unique<Texture2DArray> create(const TextureArrayProperties& properties);

		uint32_t m_id;
	protected:
		virtual void initialize(const TextureArrayProperties& properties) = 0;
	};
}

#endif // !_TEXTURE2D_H
