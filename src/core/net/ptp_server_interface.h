#ifndef _PTP_SERVER_INTERFACE_H
#define _PTP_SERVER_INTERFACE_H
#include "server_interface.h"

class PTPServerInterface final : public ServerInterface
{
public:
	PTPServerInterface(ENetHost* host, ClientInterface* client);

	void send_packet(const Packet& packet) override;
	void broadcast(const Packet& packet) override;
};

#endif // !_PTP_SERVER_INTERFACE_H
