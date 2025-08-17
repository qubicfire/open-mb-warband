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
	friend class ServerInterface;
	friend class SingleServerInterface;
public:
	void update();

	static bool connect(const std::string& ip,
		const uint16_t port,
		const ClientType type = ClientType::Client);
	static void disconnect();

	ENetPeer* get_peer() const;
private:
	void handle_message(const ServerPackets type, uint8_t* packet_info);

	void disconnect_internal();

	static void instantiate();
private:
	ENetHost* m_host;
	ENetPeer* m_peer;
};

declare_global_unique_class(ClientInterface, client_interface)

#endif // !_CLIENT_INTERFACE_H
