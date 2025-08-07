#ifndef _SINGLE_SERVER_INTERFACE_H
#define _SINGLE_SERVER_INTERFACE_H
#include "server_interface.h"

// This provider doesn't use any encryption 
// to message a packet from server to client
class SingleServerInterface : public ServerInterface
{
public:
	SingleServerInterface(ENetHost* host, ClientInterface* client);
};

#endif // !_SINGLE_SERVER_INTERFACE_H
