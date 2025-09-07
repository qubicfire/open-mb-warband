#ifndef _SERVER_
#define _SERVER_
#include "core/mb.h"
#include "client.h"

#include "packet.h"

#include <deque>

class SceneTree;

enum class ServerType
{
	None,
	Single,
	PTP,
	Dedicated
};

using Ping = Packet; // didn't want to make an inheritance

struct ScenePacket : Packet
{
	ScenePacket() { set_id(ServerPackets::Scene); }

	bool m_is_world_map = false;
	// Scene's name to change on
	std::string m_name;
};

struct RejectedPacket : Packet
{
	RejectedPacket() { set_id(ServerPackets::Rejected); }

	std::string m_message;
};

class Server
{
	friend class Client;
public:
	Server(ENetHost* host, Client* client);

	void update(SceneTree* scene_tree);

	void send(Client* client, const Packet& packet, const size_t size);
	void broadcast(const Packet& packet, const size_t size);

	template <class _Tx>
	inline void broadcast(const _Tx& packet)
	{
		if constexpr (std::is_same_v<_Tx, Packet>)
			broadcast(packet, sizeof(Packet));
		else
			broadcast(packet, sizeof(_Tx));
	}

	Client* get_local_client() const;
	
	static bool connect(const ServerType type,
		const std::string& ip = "",
		const uint16_t port = {});
	static void disconnect();
	static bool is_running();
	static bool is_type(const ServerType type);
private:
	static void reset_type();

	void connect(ENetPeer* connection);
	void disconnect(ENetPeer* connection);
	void disconnect_internal();

	void handle_message(const ClientPackets type, uint8_t* packet_info);
private:
	static inline ServerType m_type = ServerType::None;
	
	ENetHost* m_host;
	Client* m_client;

	std::list<ENetPeer*> m_connections;
};

declare_global_unique_class(Server, server)

#endif // !_SERVER_INTERFACE_H
