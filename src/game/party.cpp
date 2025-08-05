#include "core/graphics/renderer.h"
#include "core/managers/assets.h"
#include "core/managers/batch3d.h"

#include "party.h"

void Party::draw()
{
	static Shader* shader = g_assets->get_shader("main");

	// It might be possible that the icon won't be load so check it anyway
	if (!m_flags.is_flag_set(PartyFlags::pf_disabled) && m_icon)
		draw_internal(shader);
}

void Party::set_icon(MapIcon* icon) noexcept
{
	m_icon = icon;
}

void Party::set_flags(const FlagStorage<PartyFlags>& flags) noexcept
{
	m_flags = flags;
}

void Party::draw_internal(Shader* shader)
{
	//Batch3D::apply_mesh<Party>(m_origin, m_rotation, m_scale, m_icon->m_mesh);

	Renderer::prepare_model_projection(shader, m_origin, m_rotation, m_scale, m_angle);

	m_icon->m_texture->bind();

	Renderer::draw_indexed(m_icon->m_array);
}
