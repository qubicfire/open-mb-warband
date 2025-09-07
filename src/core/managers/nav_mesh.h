#ifndef _NAV_MESH_H
#define _NAV_MESH_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

struct NavPath
{

};

class NavMesh
{
public:
	NavPath build_path(const glm::vec3& origin);
	void generate(const std::vector<glm::vec3>& vertices);
};

#endif // !_NAV_MESH_H
