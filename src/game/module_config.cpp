#include "module_config.h"

#include "core/managers/assets.h"
#include "core/io/file_stream_reader.h"

void ModuleConfig::initialize()
{
	FileStreamReader stream {};
	const size_t length = stream.open("test/module.ini");

	while (stream.get_offset() < length)
	{
		const auto key = stream.read<std::string_view>();

		if (key == "load_resource")
		{
			const auto name = stream.read_until();
			g_assets->load_resource_async("test/" + name);
		}
		else
		{
			const float value = stream.number_from_chars<float>();
			m_properties.emplace(key, value);
		}
	}

	g_assets->wait_async_signal();

	log_success("module.ini loaded!");
}
