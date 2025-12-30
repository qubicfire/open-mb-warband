#ifndef _NAV_MESH_H
#define _NAV_MESH_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include <Detour/DetourNavMesh.h>
#include <Detour/DetourNavMeshQuery.h>
#include <Detour/DetourCommon.h>
#include <DetourCrowd/DetourCrowd.h>

#include "brf/face.h"

class NavMesh final
{
public:
	~NavMesh();

	bool generate(const std::vector<glm::vec3>& vertices, 
		const std::vector<Face>& indices);

	void update();

	bool request_agent_set_target(const int index, const glm::vec3& origin);
	
	int add_agent(const glm::vec3& origin);
	void remove_agent(const int index);

	const dtCrowdAgent* get_agent(const int index);
private:
	dtCrowd* m_agents;
	dtNavMesh* m_nav_mesh;
	const dtNavMeshQuery* m_nav_query;
	dtQueryFilter m_filter;
	dtCrowdAgentParams m_params;
};

declare_global_class(NavMesh, navmesh)

#endif // !_NAV_MESH_H
