#ifndef _PTP_SERVER_INTERFACE_H
#define _PTP_SERVER_INTERFACE_H
#include "server_interface.h"

class PTPServerInterface final : public ServerInterface
{
public:
	PTPServerInterface(ENetHost* host, ClientInterface* client);

	void send(const Packet& packet, const size_t size) override;
	void broadcast(const Packet& packet, const size_t size) override;
};

#endif // !_PTP_SERVER_INTERFACE_H
