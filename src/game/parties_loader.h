#ifndef _PARTIES_LOADER_H
#define _PARTIES_LOADER_H
#include "game/objects/map.h"
#include "map_icons_loader.h"

class PartiesLoader final
{
public:
	bool load(Map* map, MapIconsLoader& icons_loader);
};

#endif // !_PARTIES_LOADER_H
