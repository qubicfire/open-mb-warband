#include "utils/profiler.h"

#include "server_interface.h"
#include "single_server_interface.h"
#include "ptp_server_interface.h"

ServerInterface::ServerInterface(ENetHost* host, ClientInterface* client)
    : m_host(host)
    , m_client(client)
{
}

void ServerInterface::update_client_events()
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

void ServerInterface::dispose()
{
    m_client->dispose();

    enet_host_destroy(m_host);
}

bool ServerInterface::connect(const std::string& ip, 
    const uint16_t port,
    const ServerType type)
{
    profiler_start(server_profiler);

    if (g_server_interface)
        g_server_interface->dispose();

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

    ClientInterface::connect(ip, port, ClientType::Host);
    ClientInterface* interface = g_client_interface.get();

    if (!interface)
        return false;

    profiler_stop(server_profiler);
    log_success("Server started successfuly");

    switch (type)
    {
        case ServerType::Single:
        {
            g_server_interface = create_unique<SingleServerInterface>(host, interface);
            return true;
        }
        case ServerType::PTP:
        {
            g_server_interface = create_unique<PTPServerInterface>(host, interface);
            return true;
        }
        case ServerType::Dedicated:
        {
            return false;
        }
    }

    log_alert("Failed to create server provider, because provider type is invalid."
        "Please choose type that implemented");

    return false;
}
