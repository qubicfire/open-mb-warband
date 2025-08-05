#include "client.h"

void Client::update_server_events()
{
    bool connected = false;
    ENetEvent event;
    while (enet_host_service(m_host, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            printf("Connected to server\n");
            connected = true;
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
        }
    }

    if (connected)
    {
        if (m_server->state == ENET_PEER_STATE_CONNECTED)
        {
            ENetPacket* packet =
                enet_packet_create("HELLO WORLD", strlen("HELLO WORLD"),
                    ENET_PACKET_FLAG_RELIABLE);

            enet_peer_send(m_server, 0, packet);
        }
    }
}

Unique<Client> Client::connect(const ClientType type,
    ENetAddress* address,
    const std::string& ip)
{
    Unique<Client> client = create_unique<Client>();

    client->m_host = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!client->m_host)
    {
        log_alert("Failed to create a client. Unable to connect to the server %d", GetLastError());
        return nullptr;
    }

    enet_address_set_host(address, ip.c_str());
    client->m_server = enet_host_connect(client->m_host, address, 2, 0);

    if (!client->m_server)
    {
        log_alert("Failed connect to the server. %d", GetLastError());

        enet_host_destroy(client->m_host);
        return nullptr;
    }

    log_success("Client connected");

    return client;
}
