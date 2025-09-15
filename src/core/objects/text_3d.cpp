#include "core/graphics/renderer.h"
#include "core/graphics/text_builder_3d.h"

#include "text_3d.h"

void Text3D::set_text(const std::string& text, const float scale)
{
	add_mesh(g_text_builder_3d->construct(text));
	add_texture(g_text_builder_3d->get_texture());

	m_scale = glm::vec3(scale);
}
