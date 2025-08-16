#include "ptp_server_interface.h"

PTPServerInterface::PTPServerInterface(ENetHost* host, ClientInterface* client)
	: ServerInterface(host, client)
{
}

void PTPServerInterface::send(const Packet& packet, const size_t size)
{
	
}

void PTPServerInterface::broadcast(const Packet& packet, const size_t size)
{
	for (const auto& client : m_connections)
	{
		ENetPacket* net_packet = enet_packet_create(&packet,
			size,
			ENET_PACKET_FLAG_RELIABLE);

		enet_peer_send(client, 0, net_packet);
	}
}

