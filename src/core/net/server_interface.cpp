#include "utils/profiler.h"
#include "utils/assert.h"

#include "server_interface.h"
#include "single_server_interface.h"
#include "ptp_server_interface.h"

ServerInterface::ServerInterface(ENetHost* host, ClientInterface* client)
    : m_host(host)
    , m_client(client)
{
}

void ServerInterface::update()
{
    if (ServerInterface::is_single_state())
        return;

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
                int8_t type = 0;
                // Reading packet id
                std::memcpy(&type, event.packet->data, sizeof(int8_t));

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

void ServerInterface::disconnect()
{
    if (!g_server_interface)
        return;

    g_server_interface->disconnect_internal();
    g_server_interface.reset();
}

bool ServerInterface::connect(const std::string& ip, 
    const uint16_t port,
    const ServerType type)
{
    ServerInterface::disconnect();

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

    if (type != ServerType::Dedicated)
    {
        ClientInterface::connect(ip, port, ClientType::Host);
        ClientInterface* client = g_client_interface.get();

        if (!client)
            return false;

        g_server_interface = ServerInterface::instantiate(host, client, type);
    }
    else
    {
        g_server_interface = ServerInterface::instantiate(host, nullptr, type);
    }

    m_type = type;

    return false;
}

bool ServerInterface::connect_single()
{
    ServerInterface::disconnect();
    ClientInterface::disconnect();

    ClientInterface::instantiate();

    g_server_interface = ServerInterface::instantiate(nullptr, 
        g_client_interface.get(),
        ServerType::Single);

    m_type = ServerType::Single;

    return true;
}

bool ServerInterface::is_single_state()
{
    return m_type == ServerType::Single;
}

ClientInterface* ServerInterface::get_local_client() const
{
    return m_client;
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

Unique<ServerInterface> ServerInterface::instantiate(ENetHost* host, 
    ClientInterface* client,
    const ServerType type)
{
    switch (type)
    {
    case ServerType::Single:
        return create_unique<SingleServerInterface>(host, client);
    case ServerType::PTP:
        return create_unique<PTPServerInterface>(host, client);
    case ServerType::Dedicated:
        return nullptr;
    }

    core_assert_immediatly("Failed to create server provider, because provider type is invalid."
        " Please choose type that implemented");
    return nullptr;
}

void ServerInterface::disconnect_internal()
{
    enet_host_destroy(m_host);
}
