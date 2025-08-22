#include "core/managers/objects.h"

#include "test.h"

struct ServerTestPacket : Packet
{
	ServerTestPacket(const glm::vec3& origin = glm::vec3(0.0f)) noexcept
	{
		set_id(ServerPackets::Object);
		m_type = 0;
		m_origin = origin;
	}

	int8_t m_type;
	glm::vec3 m_origin;
};

bind_object_factory(test, Test)

#include <GL/glew.h>

void Test::client_start()
{
	load("map_flag_01");
}

void Test::start()
{
	m_origin = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Test::server_send_packet()
{
	ServerTestPacket packet(m_origin);

	g_server_interface->broadcast(packet);
}

void Test::client_receive_packet(uint8_t* packet_info)
{
	ServerTestPacket test_packet = cast_packet<ServerTestPacket>(packet_info);

	m_origin = test_packet.m_origin;
}
