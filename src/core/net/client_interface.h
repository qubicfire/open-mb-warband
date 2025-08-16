#ifndef _CLIENT_INTERFACE_H
#define _CLIENT_INTERFACE_H
#include "core/mb.h"
#include "packet.h"

enum class ClientType
{
	Host,
	Client
};

class ClientInterface final
{
public:
	void update();

	void dispose();

	static bool connect(const std::string& ip,
		const uint16_t port,
		const ClientType type = ClientType::Client);
private:
	void handle_message(const ServerPackets type, ENetEvent& event);
private:
	ENetHost* m_host;
	ENetPeer* m_peer;
};

declare_global_unique_class(ClientInterface, client_interface)

#endif // !_CLIENT_INTERFACE_H
