#ifndef _CLIENT_H
#define _CLIENT_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

#include <enet/enet.h>

enum class ClientType
{
	Owner,
	Client
};

class Client
{
public:
	void update_server_events();

	static Unique<Client> connect(const ClientType type,
		ENetAddress* address,
		const std::string& ip);
private:
	ENetHost* m_host;
	ENetPeer* m_server;
};

#endif // !_CLIENT_H
