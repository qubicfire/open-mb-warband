#ifndef _TEST_H
#define _TEST_H
#include "core/objects/object.h"

class Test : public Object, public NetworkListener
{
public:
	void start() override;

	Packet& server_build_packet();
	void client_receive_packet(const Packet& packet);
private:
	NetworkField<int> m_test;
};

#endif // !_TEST_H
