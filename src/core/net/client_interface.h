#ifndef _CLIENT_INTERFACE_H
#define _CLIENT_INTERFACE_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include <enet/enet.h>

enum class ClientType
{
	Host,
	Client
};

class ClientInterface
{
public:
	void update_server_events();

	void dispose();

	static bool connect(const std::string& ip,
		const uint16_t port,
		const ClientType type = ClientType::Client);
private:
	ENetHost* m_host;
	ENetPeer* m_peer;
};

declare_unique_class(ClientInterface, client_interface)

#endif // !_CLIENT_INTERFACE_H
