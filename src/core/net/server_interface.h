#ifndef _SERVER_INTERFACE_H
#define _SERVER_INTERFACE_H
#include "core/mb.h"
#include "client_interface.h"

#include "packet.h"

#include <deque>

enum class ServerType
{
	Single,
	PTP,
	Dedicated
};

using Ping = Packet; // didn't want to make an inheritance

struct MessagePacket : public Packet
{
	inline MessagePacket() noexcept
	{
		set_id(ServerPackets::Message);
	}

	std::string m_message;
};

class ServerInterface
{
public:
	ServerInterface(ENetHost* host, ClientInterface* client);

	void update_client_events();

	virtual void send(const Packet& packet, const size_t size) { };
	virtual void broadcast(const Packet& packet, const size_t size) { };

	template <class _Tx>
	inline void broadcast(const _Tx& packet)
	{
		if constexpr (std::is_same_v<_Tx, Packet>)
			broadcast(packet, sizeof(Packet));
		else
			broadcast(packet, sizeof(_Tx));
	}

	void dispose();

	static bool connect(const std::string& ip, 
		const uint16_t port,
		const ServerType type);
protected:
	void connect(ENetPeer* connection);
	void disconnect(ENetPeer* connection);

	void handle_message(const ClientPackets type, ENetEvent& event);
private:
	static Unique<ServerInterface> create(ENetHost* host,
		ClientInterface* client, 
		const ServerType type);
protected:
	ENetHost* m_host;
	ClientInterface* m_client;
	std::deque<ENetPeer*> m_connections;
};

declare_global_unique_class(ServerInterface, server_interface)

#endif // !_SERVER_INTERFACE_H
