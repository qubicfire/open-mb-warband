#ifndef _SERVER_PROVIDER_H
#define _SERVER_PROVIDER_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include "client.h"
#include "packet.h"

#include <enet/enet.h>

enum class ServerProviderType
{
	Single,
	PTP,
	Dedicated
};

class ServerProvider
{
public:
	ServerProvider(ENetHost* host, Unique<Client>& client);

	void update_client_events();

	virtual void send_packet(const Packet& packet) { };
	virtual void broadcast(const Packet& packet) { };

	void close();

	static bool create_server(ServerProviderType type, 
		const std::string& ip, 
		const uint16_t port);
	static Unique<Client> connect(const std::string& ip, const uint16_t port);
protected:
	ENetHost* m_host;
	Unique<Client> m_owner;
	std::vector<ENetPeer*> m_clients;
};

declare_unique_class(ServerProvider, server_provider)

#endif // !_SERVER_PROVIDER_H
