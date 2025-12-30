#include "nav_agent.h"

#include "core/managers/nav_mesh.h"

NavAgent::~NavAgent()
{
	g_navmesh->remove_agent(m_agent_id);
}

void NavAgent::initialize(const glm::vec3& origin)
{
	m_agent_id = g_navmesh->add_agent(origin);
	m_agent = g_navmesh->get_agent(m_agent_id);
}

void NavAgent::request_set_target(const glm::vec3& origin)
{
	g_navmesh->request_agent_set_target(m_agent_id, origin);
}

bool NavAgent::has_valid_path() const
{
	return m_agent->partial;
}

glm::vec3 NavAgent::get_origin() const
{
	return glm::vec3(m_agent->npos[0], m_agent->npos[1], m_agent->npos[2]);
}
