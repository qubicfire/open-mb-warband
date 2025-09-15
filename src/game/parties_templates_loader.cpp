#include "utils/mb_bit_set.h"

#include "parties_templates_loader.h"

bool PartiesTemplatesLoader::load()
{
	FileStreamReader stream{};
	if (!stream.open("test/parties.txt"))
		return false;

	const int templates = load_descriptor(stream);
	if (templates <= 0)
		return false;

	for (int i = 0; i < templates; i++)
	{
		std::string id = "pt_" + stream.read_until();
		std::string name = stream.read_until();
		mb_bit_set<int> flags = stream.number_from_chars<uint32_t>();
		int menu = stream.number_from_chars<int>();
		int faction = stream.number_from_chars<int>();
		int personality = stream.number_from_chars<int>();

		int stacks = stream.number_from_chars<int>();
		for (int j = 0; j < stacks; j++)
		{
			int troop_id = stream.number_from_chars<int>();
			int min_troops = stream.number_from_chars<int>();
			int max_troops = stream.number_from_chars<int>();
			const uint32_t member_flags = stream.number_from_chars<uint32_t>();
		}
	}

	return true;
}

int PartiesTemplatesLoader::load_descriptor(FileStreamReader& stream) const
{
	const auto file_id = stream.read<std::string_view>();
	const auto version_word = stream.read<std::string_view>();
	const auto version = stream.read<std::string_view>();

	if (file_id != "partytemplatesfile" || version_word != "version" || version != "1")
		return 0;

	return stream.number_from_chars<int>();
}
