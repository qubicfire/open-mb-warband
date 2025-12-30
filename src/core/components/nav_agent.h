#ifndef _NAV_AGENT_H
#define _NAV_AGENT_H
#include "core/mb.h"

#include <DetourCrowd/DetourCrowd.h>

class NavAgent final
{
public:
	~NavAgent();

	void initialize(const glm::vec3& origin);

	void request_set_target(const glm::vec3& origin);

	bool has_valid_path() const;
	glm::vec3 get_origin() const;
private:
	const dtCrowdAgent* m_agent;
	int m_agent_id;
};

#endif // !_NAV_AGENT_H
