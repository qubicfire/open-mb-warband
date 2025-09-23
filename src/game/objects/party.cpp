#include "core/graphics/renderer.h"

#include "party.h"

void Party::set_text_3d(Text3D* text_3d)
{
	m_text_3d = text_3d;
}

void Party::set_flags(const mb_bit_set<PartyFlags>& flags)
{
	m_flags = flags;
}

Text3D* Party::get_text_3d() const
{
	return m_text_3d;
}
