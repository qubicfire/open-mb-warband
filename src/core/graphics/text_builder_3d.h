#ifndef _TEXT_BUILDER_3D_H
#define _TEXT_BUILDER_3D_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include "core/platform/texture2d.h"
#include "core/platform/vertex_array.h"

class TextBuilder3D
{
	struct FontSettings
	{
		int m_width;
		int m_height;
		int m_padding;
		int m_size;
		int m_scale;
		float m_line_spacing;
	};

	struct CharacterSettings
	{
		int m_page; // ?
		int m_x;
		int m_y;
		float m_width;
		float m_height;
		float m_pre_shift;
		float m_post_shift;
		int m_y_adjust;
	};
public:
	bool initialize();

	brf::Mesh* construct(const std::string& text, AABB& aabb);

	mbcore::Texture2D* get_texture() const;
private:
	FontSettings m_font_settings;
	mtd::hash_map<char, CharacterSettings> m_chars;
	mbcore::Texture2D* m_font;
};

declare_global_unique_class(TextBuilder3D, text_builder_3d)

#endif // !_TEXT_BUILDER_3D_H
