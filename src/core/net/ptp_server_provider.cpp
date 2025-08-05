#include "ptp_server_provider.h"

PTPServerProvider::PTPServerProvider(ENetHost* host, Unique<Client>& client)
	: ServerProvider(host, client)
{
}

void PTPServerProvider::send_packet(const Packet& packet)
{
}

void PTPServerProvider::broadcast(const Packet& packet)
{
}

