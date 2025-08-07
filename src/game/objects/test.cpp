#include "test.h"

struct ServerTestPacket : Packet
{
	glm::vec3 m_origin;
};

void Test::start()
{
	m_test.set(this);
}

Packet& Test::server_send_packet()
{
	ServerTestPacket packet {};
	packet.m_id = get_id();
	packet.m_origin = m_origin;

	return packet;
}

void Test::client_receive_packet(const Packet& packet)
{
	ServerTestPacket test_packet = cast_packet<ServerTestPacket>(packet);

	m_origin = test_packet.m_origin;
}
