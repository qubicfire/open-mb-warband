#include "utils/profiler.h"

#include "server_provider.h"
#include "single_server_provider.h"
#include "ptp_server_provider.h"

ServerProvider::ServerProvider(ENetHost* host, Unique<Client>& client)
    : m_host(host)
    , m_owner(std::move(client))
{
}

void ServerProvider::update_client_events()
{
    ENetEvent event;
    while (enet_host_service(m_host, &event, 2) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                m_clients.push_back(event.peer);
                printf("A client has connected!\n");
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                printf("Message recieved! %s\n", event.packet->data);
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                printf("A client has disconnected\n");
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
}

void ServerProvider::close()
{
    enet_host_destroy(m_host);
}

bool ServerProvider::create_server(ServerProviderType type,
    const std::string& ip, 
    const uint16_t port)
{
    profiler_start(server_profiler);

    if (g_server_provider)
        g_server_provider->close();

    if (enet_initialize() != 0)
    {
        log_alert("Failed to initalize enet network system. Unable to create a server");
        return false;
    }

    ENetAddress address {};
    address.host = ENET_HOST_ANY;
    address.port = port;

    ENetHost* host = enet_host_create(&address, 8, 2, 0, 0);
    if (!host)
    {
        log_alert("Failed to initalize enet network system. Unable to create a server");
        return false;
    }

    Unique<Client> client = Client::connect(ClientType::Owner, &address, ip);
    if (!client)
        return false;

    server_profiler.stop();
    log_success("Server started successfuly");

    switch (type)
    {
        case ServerProviderType::Single:
        {
            g_server_provider = create_unique<SingleServerProvider>(host, client);
            return true;
        }
        case ServerProviderType::PTP:
        {
            g_server_provider = create_unique<PTPServerProvider>(host, client);
            return true;
        }
        case ServerProviderType::Dedicated:
        {
            return false;
        }
    }

    log_alert("Failed to create server provider, because provider type is invalid."
        "Please choose type that implemented");

    return false;
}

Unique<Client> ServerProvider::connect(const std::string& ip, const uint16_t port)
{
    if (enet_initialize() != 0)
    {
        log_alert("Failed to initalize enet network system. Unable connect to the server");
        return nullptr;
    }

    ENetAddress address {};
    address.host = ENET_HOST_ANY;
    address.port = port;

    return Client::connect(ClientType::Client, &address, ip);
}
