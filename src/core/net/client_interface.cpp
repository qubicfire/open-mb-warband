#include "core/managers/objects.h"

#include "client_interface.h"
#include "server_interface.h"

#include "core/engine/scene_tree.h"

void ClientInterface::update(SceneTree* scene_tree)
{
    scene_tree->client_update();

    if (ServerInterface::is_single_state())
        return;

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

void ClientInterface::disconnect_internal()
{
    enet_peer_disconnect(m_peer, 0);
    enet_host_destroy(m_host);

    enet_deinitialize();

    log_print("A client has disconnected");
}

void ClientInterface::instantiate()
{
    g_client_interface = create_unique<ClientInterface>();
}

bool ClientInterface::connect(const std::string& ip,
    const uint16_t port,
    const ClientType type)
{
    if (ServerInterface::is_single_state())
    {
        ServerInterface::disconnect();
        ServerInterface::reset_state();
    }
    else
    {
        ClientInterface::disconnect();
    }

    if (type != ClientType::Host && enet_initialize() != 0)
    {
        log_alert("Failed to initalize enet network system. Unable connect to the server");
        return false;
    }

    ENetAddress address {};
    address.host = ENET_HOST_ANY;
    address.port = port;

    g_client_interface->m_host = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!g_client_interface->m_host)
    {
        log_alert("Failed to create a client. Unable to connect to the server %d", GetLastError());
        return false;
    }

    enet_address_set_host(&address, ip.c_str());
    g_client_interface->m_peer = enet_host_connect(g_client_interface->m_host, 
        &address, 
        2,
        0);

    if (!g_client_interface->m_peer)
    {
        log_alert("Failed connect to the server");

        enet_host_destroy(g_client_interface->m_host);
        return false;
    }

    return true;
}

void ClientInterface::disconnect()
{
    if (!g_client_interface)
        return;

    g_client_interface->disconnect_internal();
}

ENetPeer* ClientInterface::get_peer() const
{
    return m_peer;
}

void ClientInterface::handle_message(const ServerPackets type, uint8_t* packet_info)
{
    switch (type)
    {
        case ServerPackets::Message:
        {
            MessagePacket packet = cast_packet<MessagePacket>(packet_info);
            break;
        }
        case ServerPackets::Object:
        {
            Object* object = g_objects->find(1);
            object->client_receive_packet(packet_info);
            break;
        }
        case ServerPackets::Kicked:
            disconnect_internal();
            break;
    }
}
