#ifndef _TEST_H
#define _TEST_H
#include "core/objects/prop.h"

class Test : public Prop
{
	object_base_impl(Test)
public:
	void client_start() override;
	void server_start() override;

	void server_send_packet();
	void client_receive_packet(uint8_t* packet_info);
};

#endif // !_TEST_H
