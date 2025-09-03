#include "utils/profiler.h"
#include "utils/assert.h"

#include "server.h"

#include "core/engine/scene_tree.h"

Server::Server(ENetHost* host, Client* client)
    : m_host(host)
    , m_client(client)
{
}

void Server::update(SceneTree* scene_tree)
{
    scene_tree->update();

    if (Server::is_type(ServerType::Single))
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

                handle_message(static_cast<ClientPackets>(type), event.packet->data);

                log_print("Packet type (ClientPackets): %d", type);
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

void Server::send(Client* client,
    const Packet& packet, 
    const size_t size)
{
    if (m_type != ServerType::Single)
    {
        ENetPacket* net_packet = enet_packet_create(&packet,
            size,
            ENET_PACKET_FLAG_RELIABLE);

        enet_peer_send(client->get_peer(), 0, net_packet);
    }
    else
    {
        // Emit sending messages to client
        uint8_t* info = reinterpret_cast<uint8_t*>(&const_cast<Packet&>(packet));
        client->handle_message(static_cast<ServerPackets>(packet.m_id), info);
    }
}

void Server::broadcast(const Packet& packet, const size_t size)
{
    if (m_type != ServerType::Single)
    {
        for (const auto& client : m_connections)
        {
            ENetPacket* net_packet = enet_packet_create(&packet,
                size,
                ENET_PACKET_FLAG_RELIABLE);

            enet_peer_send(client, 0, net_packet);
        }
    }
    else
    {
        // Emit sending messages to client
        uint8_t* info = reinterpret_cast<uint8_t*>(&const_cast<Packet&>(packet));
        g_client->handle_message(static_cast<ServerPackets>(packet.m_id), info);
    }
}

void Server::disconnect()
{
    if (!g_server)
        return;

    g_server->disconnect_internal();
    g_server.reset();
}

bool Server::is_running()
{
    return !is_type(ServerType::None);
}

bool Server::is_type(const ServerType type)
{
    return m_type == type;
}

bool Server::connect(const ServerType type,
    const std::string& ip,
    const uint16_t port)
{
    Server::disconnect();

    if (type != ServerType::Single)
    {
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
            if (!Client::connect(ClientType::PTP, ip, port))
                return false;

            construct_global_unique(Server, server, host, g_client.get());
        }
        else
        {
            construct_global_unique(Server, server, host, nullptr);
        }
    }
    else
    {
        Client::connect(ClientType::Single);

        construct_global_unique(Server, server, nullptr, g_client.get());
    }

    m_type = type;

    return false;
}

void Server::reset_type()
{
    m_type = ServerType::None;
}

Client* Server::get_local_client() const
{
    return m_client;
}

void Server::connect(ENetPeer* connection)
{
    const auto& it = std::find_if(
        m_connections.begin(),
        m_connections.end(),
        [connection](ENetPeer* client) -> bool {
            return connection->address.host.u.Byte == client->address.host.u.Byte;
        }
    );

    if (it == m_connections.end())
    {
        m_connections.push_back(connection);

        log_success("Client connected: %d", connection->incomingSessionID);
    }
    else
    {
        log_success("Connection rejected: %d", connection->incomingSessionID);

        RejectedPacket packet;
        packet.m_message = "You are already connected to this server";

        enet_peer_send(
            connection, 
            0, 
            enet_packet_create(&packet, sizeof(RejectedPacket), ENET_PACKET_FLAG_RELIABLE)
        );
    }
}

void Server::disconnect(ENetPeer* connection)
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

void Server::handle_message(const ClientPackets type, uint8_t* packet_info)
{
    switch (type)
    {
        case ClientPackets::Ping:
            break;
        case ClientPackets::MapOrigin:

            break;
    }
}

void Server::disconnect_internal()
{
    enet_host_destroy(m_host);
}
