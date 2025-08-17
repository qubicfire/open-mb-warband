#ifndef _SINGLE_SERVER_INTERFACE_H
#define _SINGLE_SERVER_INTERFACE_H
#include "server_interface.h"

// This provider doesn't use any encryption 
// to message a packet from server to client
class SingleServerInterface : public ServerInterface
{
public:
	SingleServerInterface(ENetHost* host, ClientInterface* client);

	void send(ClientInterface* client, const Packet& packet, const size_t size) override;
	void broadcast(const Packet& packet, const size_t size) override;
};

#endif // !_SINGLE_SERVER_INTERFACE_H
