#include "single_server_interface.h"
#include "client_interface.h"

SingleServerInterface::SingleServerInterface(ENetHost* host, ClientInterface* client)
	: ServerInterface(host, client)
{
}

void SingleServerInterface::send(ClientInterface* client, const Packet& packet, const size_t size)
{
	// Emit sending messages to client
	uint8_t* info = reinterpret_cast<uint8_t*>(&const_cast<Packet&>(packet));
	client->handle_message(static_cast<ServerPackets>(packet.m_id), info);
}

void SingleServerInterface::broadcast(const Packet& packet, const size_t size)
{
	// Emit sending messages to client
	uint8_t* info = reinterpret_cast<uint8_t*>(&const_cast<Packet&>(packet));
	g_client_interface->handle_message(static_cast<ServerPackets>(packet.m_id), info);
}
