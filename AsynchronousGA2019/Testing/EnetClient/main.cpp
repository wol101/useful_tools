#include "TCPIPMessage.h"

#include <enet/enet.h>

#include <QCoreApplication>

#include <string>
#include <vector>
#include <iostream>

int main(int argc, char *argv[])
{
    ENetAddress address = {};
    ENetHost *client = nullptr;
    ENetPeer *peer = nullptr;
    ENetEvent event = {};

    const char *addressString = "localhost";
    if (argc > 1) addressString = argv[1];

    // a. Initialize enet
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occured while initializing ENet.\n");
        return EXIT_FAILURE;
    }

    atexit(enet_deinitialize);

    // b. Create a host using enet_host_create
    ENetAddress *enetAddress = nullptr;
    size_t peerCount = 1;
    size_t channelLimit = 1;
    enet_uint32 incomingBandwidth = 0;
    enet_uint32 outgoingBandwidth = 0;
    client = enet_host_create(enetAddress, peerCount, channelLimit, incomingBandwidth, outgoingBandwidth);

    if (client == nullptr)
    {
        fprintf(stderr, "An error occured while trying to create an ENet server host\n");
        exit(EXIT_FAILURE);
    }

    enet_address_set_host(&address, addressString);
    address.port = 8086;

    // c. Connect and user service
    size_t channelCount = 1;
    enet_uint32 data = 0;
    peer = enet_host_connect(client, &address, channelCount, data);

    if (peer == nullptr)
    {
        fprintf(stderr, "No available peers for initializing an ENet connection\n");
        exit(EXIT_FAILURE);
    }

    int eventStatus = 1;
    time_t currentTime = time(nullptr);
    time_t lastTime = currentTime;
    std::string receivedData;
    std::vector<double> receivedGenome;
    bool connected = false;
    size_t outputCount = 0;

    while (1)
    {
        eventStatus = enet_host_service(client, &event, 0);

        // If we had some event that interested us
        if (eventStatus > 0)
        {
            switch(event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                printf("(Client) We got a new connection from %x\n", event.peer->address.host);
                connected = true;
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                if (event.packet->dataLength >= sizeof(TCPIPMessage))
                {
                    TCPIPMessage *tcpMessage = reinterpret_cast<TCPIPMessage *>(event.packet->data);
                    if (strcmp(tcpMessage->text, "reqxml") == 0)
                    {
                        receivedData = std::string(reinterpret_cast<char *>(event.packet->data) + sizeof(TCPIPMessage), event.packet->dataLength);
                        printf("(reqxml) Message from server : %s\n", receivedData.c_str());
                    }
                    if (strcmp(tcpMessage->text, "reqjob") == 0)
                    {
                        printf("(reqjob) Message from server :\n");
                        size_t genomeSize = (event.packet->dataLength - sizeof(TCPIPMessage)) / sizeof(double);
                        double *dataPtr = reinterpret_cast<double *>(event.packet->data + sizeof(TCPIPMessage));
                        receivedGenome = std::vector<double>(dataPtr, dataPtr + genomeSize);
                        for (size_t i = 0; i < receivedGenome.size(); i += 10)
                        {
                            printf("%lu", static_cast<unsigned long>(i));
                            for (size_t j = 0; j < 10; j++)
                            {
                                if (i + j >= receivedGenome.size()) break;
                                printf(" %g", receivedGenome[i + j]);
                            }
                            printf("\n");
                        }
                    }
                }
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("(Client) %s disconnected.\n", reinterpret_cast<char *>(event.peer->data));
                // Reset client's information
                event.peer->data = nullptr;
                connected = false;
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
            }
        }

        currentTime = time(nullptr);
        time_t deltaTime = currentTime - lastTime;
        if (deltaTime >= 5) // should be every 5 seconds
        {
            printf("(Client) last event status = %d event.type = %d \n", eventStatus, int(event.type));
            lastTime = currentTime;
            if (connected)
            {
                TCPIPMessage message = {};
                size_t packetSize = sizeof(TCPIPMessage);
                if(outputCount % 2==0) snprintf(message.text, sizeof(message.text), "reqxml");
                else snprintf(message.text, sizeof(message.text), "reqjob");
                printf("%ld (Client) sending message %s\n", static_cast<unsigned long>(outputCount), message.text);
                ENetPacket *packet = enet_packet_create(&message, packetSize, ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(peer, 0, packet);
                outputCount++;
            }
            else
            {
                printf("(Client) trying reconnect\n");
                peer = enet_host_connect(client, &address, channelCount, data);
                if (peer == nullptr)
                {
                    fprintf(stderr, "No available peers for initializing an ENet connection\n");
                }
            }
        }

    }
}
