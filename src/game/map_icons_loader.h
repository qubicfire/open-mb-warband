#ifndef _MAP_ICONS_LOADER_H
#define _MAP_ICONS_LOADER_H
#include "brf/mesh.h"

#include "core/platform/vertex_array.h"
#include "core/platform/texture2d.h"

struct MapIcon
{
	brf::Mesh* m_mesh;
	float m_scale;
	glm::vec3 m_flag_offset;
};

class MapIconsLoader final
{
public:
	bool load();

	MapIcon* get_icon(int index);
private:
	std::vector<MapIcon> m_icons;
};

#endif // !_MAP_ICONS_LOADER_H
