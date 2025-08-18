#include "core/graphics/renderer.h"
#include "core/graphics/batch3d.h"
#include "core/managers/assets.h"

#include "party.h"

void Party::draw()
{
	static Shader* shader = g_assets->get_shader("main");

	// It might be possible that the icon won't be load so check it anyway
	if (!m_flags.is_flag_set(PartyFlags::pf_disabled) && m_mesh)
		Model::draw_internal(shader);
}

void Party::set_flags(const FlagStorage<PartyFlags>& flags)
{
	m_flags = flags;
}
