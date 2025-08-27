#include "core/managers/assets.h"

#include "map_icons_loader.h"

using namespace mbcore;

bool MapIconsLoader::load()
{
	FileStreamReader stream {};
	if (!stream.open("test/map_icons.txt"))
		return false;

	const int icons_count = load_descriptor(stream);
	m_icons.reserve(icons_count);

	// https://earendil_ardamire.gitlab.io/modding-guide/Subpages/Documentation_Module_System/Module_Map_Icons.html
	for (int i = 0; i < icons_count; i++)
	{
		std::string icon_id = "icon_" + stream.read_until(' ', '\n');
		int flags = stream.number_from_chars<int>();
		std::string name = stream.read_until(' ', '\n');
		float scale = stream.number_from_chars<float>();
		int sound_id = stream.number_from_chars<int>();

		float offset_y = stream.number_from_chars<float>();
		float offset_x = stream.number_from_chars<float>();
		float offset_z = stream.number_from_chars<float>();

		brf::Mesh* mesh = g_assets->get_mesh(name);
		m_icons.push_back({ mesh, scale });
	}

	return true;
}

int MapIconsLoader::load_descriptor(FileStreamReader& stream) const
{
	const auto file_id = stream.read<std::string_view>();
	const auto version_word = stream.read<std::string_view>();
	const auto version = stream.read<std::string_view>();

	if (file_id != "map_icons_file" || version_word != "version" || version != "1")
		return 0;

	return stream.number_from_chars<int>();
}

MapIcon* MapIconsLoader::get_icon(int index)
{
	return &m_icons[index];
}
