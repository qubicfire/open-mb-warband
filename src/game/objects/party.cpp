#include "core/graphics/renderer.h"
#include "core/graphics/batch3d.h"
#include "core/managers/assets.h"

#include "party.h"

void Party::draw()
{
	static Shader* shader = g_assets->get_shader("main");

	// It might be possible that the icon won't be load so check it anyway
	if (!m_flags.is_flag_set(PartyFlags::pf_disabled) && m_mesh)
		Prop::draw_internal(shader);
}

void Party::set_text_3d(Text3D* text_3d)
{
	m_text_3d = text_3d;
}

void Party::set_flags(const FlagStorage<PartyFlags>& flags)
{
	m_flags = flags;
}

Text3D* Party::get_text_3d() const
{
	return m_text_3d;
}
