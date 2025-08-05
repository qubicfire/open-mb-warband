#ifndef _SINGLE_SERVER_PROVIDER_H
#define _SINGLE_SERVER_PROVIDER_H
#include "server_provider.h"

// This provider doesn't use any encryption 
// to message a packet from server to client
class SingleServerProvider : public ServerProvider
{
public:
	SingleServerProvider(ENetHost* host, Unique<Client>& client);
};

#endif // !_SINGLE_SERVER_PROVIDER_H
