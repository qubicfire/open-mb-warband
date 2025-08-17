#ifndef _SERVER_INTERFACE_H
#define _SERVER_INTERFACE_H
#include "core/mb.h"
#include "client_interface.h"

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

struct MessagePacket : Packet
{
	MessagePacket() { set_id(ServerPackets::Message); }

	std::string m_message;
};

struct ScenePacket : Packet
{
	ScenePacket() { set_id(ServerPackets::Scene); }

	bool m_is_world_map = false;
	// Scene's name to change on
	std::string m_name;
};

class ServerInterface
{
public:
	ServerInterface(ENetHost* host, ClientInterface* client);

	void update(SceneTree* scene_tree);

	virtual void send(ClientInterface* client, const Packet& packet, const size_t size) { };
	virtual void broadcast(const Packet& packet, const size_t size) { };

	template <class _Tx>
	inline void broadcast(const _Tx& packet)
	{
		if constexpr (std::is_same_v<_Tx, Packet>)
			broadcast(packet, sizeof(Packet));
		else
			broadcast(packet, sizeof(_Tx));
	}

	static bool connect(const std::string& ip, 
		const uint16_t port,
		const ServerType type);
	static bool connect_single();
	static void disconnect();
	static bool is_single_state();
	static bool is_valid_state();
	static void reset_state();

	ClientInterface* get_local_client() const;
protected:
	void connect(ENetPeer* connection);
	void disconnect(ENetPeer* connection);

	void handle_message(const ClientPackets type, ENetEvent& event);
private:
	static Unique<ServerInterface> instantiate(ENetHost* host,
		ClientInterface* client, 
		const ServerType type);
protected:
	void disconnect_internal();
protected:
	static inline ServerType m_type = ServerType::None;
	
	ENetHost* m_host;
	ClientInterface* m_client;

	std::deque<ENetPeer*> m_connections;
};

declare_global_unique_class(ServerInterface, server_interface)

#endif // !_SERVER_INTERFACE_H
