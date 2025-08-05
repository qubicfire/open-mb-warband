#ifndef _PTP_SERVER_PROVIDER_H
#define _PTP_SERVER_PROVIDER_H
#include "server_provider.h"

class PTPServerProvider final : public ServerProvider
{
public:
	PTPServerProvider(ENetHost* host, Unique<Client>& client);

	void send_packet(const Packet& packet) override;
	void broadcast(const Packet& packet) override;
};

#endif // !_PTP_SERVER_PROVIDER_H
