#include "utils/mb_bit_set.h"

#include "core/managers/objects.h"
#include "objects/party.h"
#include "parties_loader.h"

bool PartiesLoader::load(Map* map, MapIconsLoader& icons_loader)
{
	FileStreamReader stream {};
	if (!stream.open("test/parties.txt"))
		return false;

	const int parties = load_descriptor(stream);

	// https://earendil_ardamire.gitlab.io/modding-guide/Subpages/Documentation_Module_System/Module_Parties.html
	for (int i = 0; i < parties; i++)
	{
		Party* party = Object::instantiate<Party>();

		stream.read<std::string_view>(); // unused
		stream.read<std::string_view>(); // unused
		stream.read<std::string_view>(); // unused

		const auto id = stream.read<std::string_view>();
		const auto name = stream.read_until();
		mb_bit_set<PartyFlags> flags = stream.number_from_chars<uint32_t>();

		stream.read<std::string_view>(); // deprecated (no_menu is always zero)

		const int party_template = stream.number_from_chars<int>();
		const int faction = stream.number_from_chars<int>();
		const int personality = stream.number_from_chars<int>();
		const int ai_behavior = stream.number_from_chars<int>();
		const int ai_target_party = stream.number_from_chars<int>();

		stream.read<std::string_view>(); // unused (?)
		stream.read<std::string_view>(); // unused (?)

		stream.read<std::string_view>(); // why are they so many same strings in this file?
		stream.read<std::string_view>(); 
		stream.read<std::string_view>(); 
		stream.read<std::string_view>(); 

		const float x = stream.number_from_chars<float>();
		const float y = stream.number_from_chars<float>();

		stream.read<std::string_view>(); // skip '0.0'

		const int stacks = stream.number_from_chars<int>();

		for (int j = 0; j < stacks; j++)
		{
			const int troop_id = stream.number_from_chars<int>();
			const int troops_count = stream.number_from_chars<int>();
			stream.read<std::string_view>(); // fucking zeros everywhere
			const uint32_t member_flags = stream.number_from_chars<uint32_t>();
		}

		const float angle = stream.number_from_chars<float>();

		party->set_flags(flags);

		if (!flags.is_bit_set(PartyFlags::pf_disabled))
		{
			Text3D* text_3d = nullptr;
			bool is_static = flags.try_clear_bit(PartyFlags::pf_is_static);
			bool is_always_visible = flags.try_clear_bit(PartyFlags::pf_always_visible);

			party->set_origin(map->align_point_to_ground(x, y));

			if (flags.try_clear_bit(PartyFlags::pf_label_small))
			{
				text_3d = Object::instantiate<Text3D>();
				text_3d->set_text(name);
				text_3d->set_origin(party->get_origin() + glm::vec3(0.0, 1.0f, 0.0f));
				party->set_text_3d(text_3d);
			}
			else if (flags.try_clear_bit(PartyFlags::pf_label_medium))
			{
				text_3d = Object::instantiate<Text3D>();
				text_3d->set_text(name);
				text_3d->set_origin(party->get_origin() + glm::vec3(0.0, 1.0f, 0.0f));
				party->set_text_3d(text_3d);
			}
			else if (flags.try_clear_bit(PartyFlags::pf_label_large))
			{
				text_3d = Object::instantiate<Text3D>();
				text_3d->set_text(name);
				text_3d->set_origin(party->get_origin() + glm::vec3(0.0, 1.0f, 0.0f));
				party->set_text_3d(text_3d);
			}

			if (flags.try_clear_bit(PartyFlags::pf_hide_defenders))
			{

			}
			if (flags.try_clear_bit(PartyFlags::pf_show_faction))
			{

			}
			if (flags.try_clear_bit(PartyFlags::pf_no_label))
			{

			}
			if (flags.try_clear_bit(PartyFlags::pf_limit_members))
			{

			}

			// For whatever reason, taleworlds developers
			// used the most fucking hilarious way to store the icon id
			// The icon id stored in flags, so we have to remove every flag one by one
			int icon_id = flags.get();

			// TODO: godawn hack. 
			// there should not be a player as faction
			// I forgot why I even did it,
			// but i don't want to touch this 
			if (icon_id > 0 && faction != 13)
			{
				MapIcon* icon = icons_loader.get_icon(icon_id);

				party->load(icon->m_mesh);
				party->set_scale(glm::vec3(0.5f));
			}
			else
			{
				party->set_object_flag(Object::Flags::Invisible);
			}

			party->set_angle(glm::degrees(angle));
			party->set_rotation(glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else
		{
			party->set_object_flag(Object::Flags::Invisible);
		}
	}

	return true;
}

int PartiesLoader::load_descriptor(FileStreamReader& stream) const
{
	const auto file_id = stream.read<std::string_view>();
	const auto version_word = stream.read<std::string_view>();
	const auto version = stream.read<std::string_view>();
	if (file_id != "partiesfile" || version_word != "version" || version != "1")
		return 0;

	stream.read<std::string_view>(); // skip copy of parties count
	return stream.number_from_chars<int>();
}
