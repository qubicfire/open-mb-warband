#ifndef _MAP_ICONS_LOADER_H
#define _MAP_ICONS_LOADER_H
#include "core/io/file_stream_reader.h"
#include "utils/thread_pool.h"

#include "brf/mesh.h"
#include "core/platform/vertex_array.h"
#include "core/platform/texture2d.h"

#include "core/mb_type_traits.h"
#include "core/mb.h"

struct MapIcon
{
	Unique<mbcore::VertexArray> m_array;
	Unique<mbcore::Texture2D> m_texture;
	brf::Mesh* m_mesh;
	float m_scale;
};

class MapIconsLoader final
{
public:
	bool load(ThreadPool& pool);

	MapIcon* get_icon(int index);
private:
	int load_descriptor(FileStreamReader& stream) const;
private:
	std::vector<MapIcon> m_icons;
};

#endif // !_MAP_ICONS_LOADER_H
