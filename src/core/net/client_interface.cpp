#include "client_interface.h"
#include "server_interface.h"

void ClientInterface::update()
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
                int type = 0;
                // Reading packet id
                std::memcpy(&type, event.packet->data, sizeof(int));

                handle_message(static_cast<ServerPackets>(type), event);

                log_print("Packet type (ServerPackets): %d", type);
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                dispose();
                log_print("A client has disconnected");
                break;
        }
    }
}

void ClientInterface::dispose()
{
    enet_peer_disconnect(m_peer, 0);
    enet_host_destroy(m_host);

    enet_deinitialize();
}

bool ClientInterface::connect(const std::string& ip,
    const uint16_t port,
    const ClientType type)
{
    if (g_client_interface)
        g_client_interface->dispose();
    else
        g_client_interface = create_unique<ClientInterface>();

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

void ClientInterface::handle_message(const ServerPackets type, ENetEvent& event)
{
    switch (type)
    {
        case ServerPackets::Message:
        {
            MessagePacket packet = cast_packet<MessagePacket>(event);
            break;
        }
    }
}
