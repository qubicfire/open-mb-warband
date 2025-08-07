#include "ptp_server_interface.h"

PTPServerInterface::PTPServerInterface(ENetHost* host, ClientInterface* client)
	: ServerInterface(host, client)
{
}

void PTPServerInterface::send_packet(const Packet& packet)
{
}

void PTPServerInterface::broadcast(const Packet& packet)
{
}

