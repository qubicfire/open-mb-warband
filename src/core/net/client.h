#ifndef _CLIENT_
#define _CLIENT_
#include "core/mb.h"
#include "packet.h"

class SceneTree;

enum class ClientType
{
	Single,
	PTP,
};

enum class ClientStatus : int8_t
{
	Map,
	Scene
};

struct MapOriginPacket : Packet
{
	MapOriginPacket(const glm::vec3& origin = glm::vec3(0.0f))
		: m_origin(origin)
	{
		set_id(ClientPackets::MapOrigin);
	}

	glm::vec3 m_origin;
};

class Client
{
	friend class Server;
public:
	Client(ENetHost* host, ENetPeer* peer);

	void update(SceneTree* scene_tree);

	void send(const Packet& packet, const size_t size);

	template <class _Tx>
	inline void send(const _Tx& packet)
	{
		if constexpr (std::is_same_v<_Tx, Packet>)
			send(packet, sizeof(Packet));
		else
			send(packet, sizeof(_Tx));
	}

	static bool connect(const ClientType type,
		const std::string& ip = "",
		const uint16_t port = {});
	static void disconnect();
	static bool is_running();

	ENetPeer* get_peer() const;
private:
	void handle_message(const ServerPackets type, uint8_t* packet_info);

	void disconnect_internal();
private:
	ClientType m_type;

	ENetHost* m_host;
	ENetPeer* m_peer;

	ClientStatus m_status;
};

declare_global_unique_class(Client, client)

#endif // !_CLIENT_
