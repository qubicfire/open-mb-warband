#include "utils/assert.h"

#include "core/managers/objects.h"

#include "client.h"
#include "server.h"

#include "core/engine/scene_tree.h"

Client::Client(ENetHost* host, ENetPeer* peer)
    : m_host(host)
    , m_peer(peer)
    , m_status(ClientStatus::Map)
{
}

void Client::update(SceneTree* scene_tree)
{
    if (!Server::is_type(ServerType::Single))
    {
        ENetEvent event {};
        while (enet_host_service(m_host, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                log_success("Connected to server");
                break;
            case ENET_EVENT_TYPE_RECEIVE:
            {
                int8_t type = 0;
                // Reading packet id
                std::memcpy(&type, event.packet->data, sizeof(int8_t));

                handle_message(static_cast<ServerPackets>(type), event.packet->data);

                //log_print("Packet type (ServerPackets): %d", type);
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                disconnect_internal();
                break;
            }
        }
    }

    scene_tree->update_client();

    g_threads->wait();
}

void Client::send(const Packet& packet, const size_t size)
{
    if (m_type != ClientType::Single)
    {
        ENetPacket* net_packet = enet_packet_create(&packet,
            size,
            ENET_PACKET_FLAG_RELIABLE);

        enet_peer_send(m_peer, 0, net_packet);
    }
    else
    {
        uint8_t* info = reinterpret_cast<uint8_t*>(&const_cast<Packet&>(packet));
        g_server->handle_message(static_cast<ClientPackets>(packet.m_id), info);
    }
}

void Client::disconnect_internal()
{
    enet_peer_disconnect(m_peer, 0);
    enet_host_destroy(m_host);

    g_client->m_peer = nullptr;
    g_client->m_host = nullptr;

    enet_deinitialize();

    log_print("A client has disconnected");
}

bool Client::connect(const ClientType type,
    const std::string& ip,
    const uint16_t port)
{
    if (type == ClientType::Single)
    {
        Client::disconnect();

        construct_global_unique(Client, client, nullptr, nullptr);

        g_client->m_type = type;

        return true;
    }

    if (!Server::is_type(ServerType::None))
    {
        Server::disconnect();
        Server::reset_type();
    }

    Client::disconnect();

    if (enet_initialize() != 0)
    {
        log_alert("Failed to initalize enet network system. Unable connect to the server");
        return false;
    }

    ENetAddress address {};
    address.host = ENET_HOST_ANY;
    address.port = port;

    ENetHost* host = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!host)
    {
        log_alert("Failed to create a client. Unable to connect to the server %d", GetLastError());
        return false;
    }

    enet_address_set_host(&address, ip.c_str());
    ENetPeer* peer = enet_host_connect(host,
        &address, 
        2,
        0);

    if (!peer)
    {
        log_alert("Failed connect to the server");

        enet_host_destroy(host);
        return false;
    }

    construct_global_unique(Client, client, host, peer);

    g_client->m_type = type;

    return true;
}

void Client::disconnect()
{
    if (!g_client || !g_client->m_peer)
        return;

    g_client->disconnect_internal();
}

bool Client::is_running()
{
    return g_client != nullptr;
}

ENetPeer* Client::get_peer() const
{
    return m_peer;
}

void Client::handle_message(const ServerPackets type, uint8_t* packet_info)
{
    switch (type)
    {
        case ServerPackets::Object:
        {
            Object* object = g_objects->find(2);
            object->client_receive_packet(packet_info);
            break;
        }
        case ServerPackets::Kicked:
            disconnect_internal();
            break;
    }
}
