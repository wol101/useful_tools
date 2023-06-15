#include <enet/enet.h>

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    ENetAddress address = {};
    ENetHost *server = nullptr;
    ENetEvent event = {};
    bool respondFlag = true;
    char hostName[1024];

    int port = 8086;
    if (argc > 1) port = atoi(argv[1]);


    // a. Initialize enet
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occured while initializing ENet.\n");
        return EXIT_FAILURE;
    }

    atexit(enet_deinitialize);

    // b. Create a host using enet_host_create
    address.host = ENET_HOST_ANY;
    address.port = enet_uint16(port);

    server = enet_host_create(&address, 32, 2, 0, 0);

    if (server == nullptr)
    {
        fprintf(stderr, "An error occured while trying to create an ENet server host\n");
        exit(EXIT_FAILURE);
    }

    // c. Connect and user service
    int eventStatus = 1;

    while (1)
    {
        eventStatus = enet_host_service(server, &event, 0);

        // If we had some event that interested us
        if (eventStatus > 0)
        {
            switch(event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                enet_address_get_host_ip(&event.peer->address, hostName, sizeof(hostName));
                printf("(Server) We got a new connection from %s port %d\n", hostName, event.peer->address.port);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                enet_address_get_host_ip(&event.peer->address, hostName, sizeof(hostName));
                printf("(Server) Message from client : %s %d\n", hostName, event.peer->address.port);
                if (respondFlag)
                {
                    /* respond with a message */
                    const char *message = "Server responding\n";
                    ENetPacket *packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(event.peer, 0, packet);
                }
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", reinterpret_cast<char *>(event.peer->data));

                // Reset client's information
                event.peer->data = nullptr;
                break;

            case ENET_EVENT_TYPE_NONE:
                break;

            }
        }

    }
}
