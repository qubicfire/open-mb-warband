#ifndef _MAP_H
#define _MAP_H
#include "core/objects/object.h"
#include "core/objects/rigidbody.h"

class Map final : public Object
{
	object_base(Map)
	object_client_base(map_terrain)
public:
	struct MapVertex
	{
		glm::vec3 m_origin;
		glm::vec2 m_texture;
		float m_type;
#ifdef _DEBUG
		glm::vec3 m_color;
#endif // _DEBUG
	};

	void start() override;

	glm::vec3 align_point_to_ground(float x, float y);
private:
	RigidBody m_body;
};

#endif // !_MAP_H
