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
    ENetEvent event {};
    while (enet_host_service(m_host, &event, 2) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                connect(event.peer);
                break;
            case ENET_EVENT_TYPE_RECEIVE:
            {
                int type = 0;
                // Reading packet id
                std::memcpy(&type, event.packet->data, sizeof(int));

                handle_message(static_cast<ClientPackets>(type), event);

                log_print("Packet type (ServerPackets): %d", type);
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                disconnect(event.peer);
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
}

void ServerInterface::dispose()
{
    enet_host_destroy(m_host);
}

bool ServerInterface::connect(const std::string& ip, 
    const uint16_t port,
    const ServerType type)
{
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

    log_success("Server started successfuly");

    ClientInterface::connect(ip, port, ClientType::Host);
    ClientInterface* interface = g_client_interface.get();

    if (!interface)
        return false;

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

void ServerInterface::connect(ENetPeer* connection)
{
    m_connections.push_back(connection);

    log_success("Client connected: %d", connection->connectID);
}

void ServerInterface::disconnect(ENetPeer* connection)
{
    const auto& it = std::remove(m_connections.begin(),
        m_connections.end(),
        connection);

    if (it != m_connections.end())
    {
        m_connections.erase(it);
        log_warning("A client has disconnected");
    }
}

void ServerInterface::handle_message(const ClientPackets type, ENetEvent& event)
{
}
