#ifndef _SERVER_INTERFACE_H
#define _SERVER_INTERFACE_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include "client_interface.h"
#include "packet.h"

#include <enet/enet.h>

enum class ServerType
{
	Single,
	PTP,
	Dedicated
};

class ServerInterface
{
public:
	ServerInterface(ENetHost* host, ClientInterface* client);

	void update_client_events();

	virtual void send_packet(const Packet& packet) { };
	virtual void broadcast(const Packet& packet) { };

	void dispose();

	static bool connect(const std::string& ip, 
		const uint16_t port,
		const ServerType type);
protected:
	ENetHost* m_host;
	ClientInterface* m_client;
	std::vector<ENetPeer*> m_clients;
};

declare_unique_class(ServerInterface, server_interface)

#endif // !_SERVER_INTERFACE_H
