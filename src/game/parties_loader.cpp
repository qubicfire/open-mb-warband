#include <glm/trigonometric.hpp>
#include "utils/flag_storage.h"

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
		Party* party = g_objects->create_object<Party>();

		stream.read<std::string_view>(); // unused
		stream.read<std::string_view>(); // unused
		stream.read<std::string_view>(); // unused

		const auto id = stream.read<std::string_view>();
		const auto name = stream.read<std::string_view>();
		FlagStorage<PartyFlags> flags = static_cast<PartyFlags>(
			stream.number_from_chars<uint32_t>()
		);

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

		for (uint32_t i = 0; i < stacks; i++)
		{
			const int troop_id = stream.number_from_chars<int>();
			const int troops_count = stream.number_from_chars<int>();
			stream.read<std::string_view>(); // fucking zeros everywhere
			const uint32_t member_flags = stream.number_from_chars<uint32_t>();
		}

		const float angle = stream.number_from_chars<float>();

		FlagStorage<PartyFlags> temp_flags = flags;
		if (!temp_flags.is_flag_set(PartyFlags::pf_disabled))
		{
			bool is_static = temp_flags.try_clear_flag(PartyFlags::pf_is_static);
			bool is_always_visible = temp_flags.try_clear_flag(PartyFlags::pf_always_visible);

			if (temp_flags.try_clear_flag(PartyFlags::pf_label_small))
			{

			}
			else if (temp_flags.try_clear_flag(PartyFlags::pf_label_medium))
			{

			}
			else if (temp_flags.try_clear_flag(PartyFlags::pf_label_large))
			{

			}

			if (temp_flags.try_clear_flag(PartyFlags::pf_hide_defenders))
			{

			}
			if (temp_flags.try_clear_flag(PartyFlags::pf_show_faction))
			{

			}
			if (temp_flags.try_clear_flag(PartyFlags::pf_no_label))
			{

			}
			if (temp_flags.try_clear_flag(PartyFlags::pf_limit_members))
			{

			}

			int icon_id = temp_flags.get_storage();

			if (icon_id > 0 && faction != 13) // hack. TODO: remove later
			{
				MapIcon* icon = icons_loader.get_icon(icon_id);

				party->load(icon->m_mesh);
			}

			party->set_origin(map->align_point_to_ground(x, y));
			party->set_angle(glm::degrees(angle));
			party->set_rotation(glm::vec3(0.0f, 1.0f, 0.0f));
		}

		party->set_flags(flags);
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
