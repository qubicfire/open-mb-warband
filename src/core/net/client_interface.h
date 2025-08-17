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
	friend class SingleServerInterface;
public:
	void update();

	void disconnect();

	static bool connect(const std::string& ip,
		const uint16_t port,
		const ClientType type = ClientType::Client);

	ENetPeer* get_peer() const;
private:
	void dispose();

	void handle_message(const ServerPackets type, uint8_t* packet_info);
private:
	ENetHost* m_host;
	ENetPeer* m_peer;
};

declare_global_unique_class(ClientInterface, client_interface)

#endif // !_CLIENT_INTERFACE_H
