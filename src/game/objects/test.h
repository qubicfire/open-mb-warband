#ifndef _TEST_H
#define _TEST_H
#include "core/objects/prop.h"
#include "core/objects/text_3d.h"

#include "core/components/nav_agent.h"

class Test : public Prop
{
	object_base(Test)
public:
	void start() override;
	void start_client() override;

	void update() override;

	void server_send_packet();
	void client_receive_packet(uint8_t* packet_info);

	NavAgent* get_agent();
private:
	Text3D* m_text_3d;
	NavAgent m_agent;
};

#endif // !_TEST_H
