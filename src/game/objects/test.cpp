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

void Test::start()
{
	set_origin(glm::vec3(std::rand() % 5, 1.0f, std::rand() % 5));

	m_agent.initialize(m_origin);
}

void Test::start_client()
{
	load("map_flag_01");

	m_text_3d = Object::instantiate<Text3D>();
	m_text_3d->set_text("Wercheg");
	m_text_3d->set_origin(glm::vec3(0.0f, 2.0f, 0.0f));
}

void Test::update()
{
	if (m_agent.has_valid_path())
		set_origin(m_agent.get_origin());
}

void Test::server_send_packet()
{
	ServerTestPacket packet(m_origin);

	g_server->broadcast(packet);
}

void Test::client_receive_packet(uint8_t* packet_info)
{
	ServerTestPacket test_packet = cast_packet<ServerTestPacket>(packet_info);

	m_origin = test_packet.m_origin;
}

NavAgent* Test::get_agent()
{
	return &m_agent;
}
