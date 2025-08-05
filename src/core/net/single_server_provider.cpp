#include "single_server_provider.h"

SingleServerProvider::SingleServerProvider(ENetHost* host, Unique<Client>& client)
	: ServerProvider(host, client)
{
}
