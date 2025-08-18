#ifndef _PARTIES_LOADER_H
#define _PARTIES_LOADER_H
#include "core/io/file_stream_reader.h"
#include "game/objects/map.h"
#include "map_icons_loader.h"

class PartiesLoader final
{
public:
	bool load(Map* map, MapIconsLoader& icons_loader);
private:
	int load_descriptor(FileStreamReader& stream) const;
};

#endif // !_PARTIES_LOADER_H
