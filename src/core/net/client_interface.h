#ifndef _CLIENT_INTERFACE_H
#define _CLIENT_INTERFACE_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include <enet/enet.h>
#include "packet.h"

enum class ClientType
{
	Host,
	Client
};

class ClientInterface final
{
public:
	void update_server_events();

	void dispose();

	static bool connect(const std::string& ip,
		const uint16_t port,
		const ClientType type = ClientType::Client);
private:
	void handle_message(const ServerPackets type, ENetEvent& event);

	template <class _Tx>
	inline _Tx get_packet(ENetEvent& event)
	{
		_Tx packet {};

		std::memcpy(&packet, event.packet->data, sizeof(_Tx));

		return packet;
	}
private:
	ENetHost* m_host;
	ENetPeer* m_peer;
};

declare_unique_class(ClientInterface, client_interface)

#endif // !_CLIENT_INTERFACE_H
