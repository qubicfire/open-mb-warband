#ifndef _PARTY_H
#define _PARTY_H
#include "utils/mb_bit_set.h"

#include "core/mb_type_traits.h"
#include "core/objects/prop.h"
#include "core/objects/text_3d.h"

#include "core/managers/nav_mesh.h"

#include "game/map_icons_loader.h"

enum PartyFlags : uint32_t
{
	pf_icon_mask = 0x000000ff,
	pf_disabled = 0x00000100,
	pf_is_ship = 0x00000200,
	pf_is_static = 0x00000400,

	pf_label_small = 0x00000000,
	pf_label_medium = 0x00001000,
	pf_label_large = 0x00002000,

	pf_always_visible = 0x00004000,
	pf_default_behavior = 0x00010000,
	pf_auto_remove_in_town = 0x00020000,
	pf_quest_party = 0x00040000,
	pf_no_label = 0x00080000,
	pf_limit_members = 0x00100000,
	pf_hide_defenders = 0x00200000,
	pf_show_faction = 0x00400000,
	pf_is_hidden = 0x01000000,
	pf_dont_attack_civilians = 0x02000000,
	pf_civilian = 0x04000000,

	pf_town = pf_is_static | pf_always_visible | pf_show_faction | pf_label_large,
	pf_castle = pf_is_static | pf_always_visible | pf_show_faction | pf_label_medium,
	pf_village = pf_is_static | pf_always_visible | pf_hide_defenders | pf_label_small,
};							

class Party : public Prop
{
	static constexpr auto pf_carry_goods_bits = 48;
	static constexpr auto pf_carry_gold_bits = 56;
	static constexpr auto pf_carry_gold_multiplier = 20;
	static constexpr auto pf_carry_goods_mask = 0x00ff000000000000;
	static constexpr auto pf_carry_gold_mask = 0xff00000000000000;

	object_base(Party)
public:
	void update();

	void set_text_3d(Text3D* text_3d);
	void set_flags(const mb_bit_set<PartyFlags>& flags);

	Text3D* get_text_3d() const;
private:
	Text3D* m_text_3d;
	mb_bit_set<PartyFlags> m_flags;
};

#endif // !_PARTY_H
