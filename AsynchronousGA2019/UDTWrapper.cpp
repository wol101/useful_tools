#ifndef WIN32
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#endif
#include <iostream>

#include "UDTWrapper.h"

using namespace std::string_literals;

UDTSOCKET UDTWrapper::StartServer(const std::string &port)
{
    addrinfo addrinfoHints = {};
    addrinfo* addrinfoResult;

    addrinfoHints.ai_flags = AI_PASSIVE;
    addrinfoHints.ai_family = AF_INET;
    addrinfoHints.ai_socktype = SOCK_STREAM;
    //hints.ai_socktype = SOCK_DGRAM;

    UDTSOCKET serverSocket;
    try
    {
        if (getaddrinfo(NULL, port.c_str(), &addrinfoHints, &addrinfoResult))
            throw("StartServer getaddrinfo error: illegal port number or port is busy"s);

        serverSocket = UDT::socket(addrinfoResult->ai_family, addrinfoResult->ai_socktype, addrinfoResult->ai_protocol);
        if (serverSocket == UDT::INVALID_SOCK)
            throw("StartServer UDT::socket error"s + UDT::getlasterror().getErrorMessage());

        // UDT Options
        int sndBuf = 10000000;
        int rcvBuf = 10000000;
        bool useRendezvous = true;
        bool resuseAddr = true;
        UDT::setsockopt(serverSocket, 0, UDT_RCVBUF, &sndBuf, sizeof(int));
        UDT::setsockopt(serverSocket, 0, UDP_RCVBUF, &rcvBuf, sizeof(int));
        UDT::setsockopt(serverSocket, 0, UDT_RENDEZVOUS, &useRendezvous, sizeof(bool));
        UDT::setsockopt(serverSocket, 0, UDT_REUSEADDR, &resuseAddr, sizeof(bool));

        if (UDT::bind(serverSocket, addrinfoResult->ai_addr, int(addrinfoResult->ai_addrlen)) == UDT::ERROR)
            throw("StartServer UDT::bind error"s + UDT::getlasterror().getErrorMessage());

        if (UDT::listen(serverSocket, 10) == UDT::ERROR)
            throw("StartServer UDT::listen error"s + UDT::getlasterror().getErrorMessage());
    }
    catch (std::string errorMessage)
    {
        std::cerr << errorMessage;
        if (serverSocket != UDT::INVALID_SOCK) UDT::close(serverSocket);
        freeaddrinfo(addrinfoResult);
        return UDT::INVALID_SOCK;
    }

    freeaddrinfo(addrinfoResult);
    return serverSocket;
}

UDTSOCKET UDTWrapper::StartAcceptor(UDTSOCKET serverSocket, uint32_t *address, uint16_t *port)
{
    sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);
    UDTSOCKET acceptorSocket = UDT::accept(serverSocket, (sockaddr*)&clientaddr, &addrlen);
    // these assume IP4 connections
    *address = ntohl(reinterpret_cast<struct sockaddr_in *>(&clientaddr)->sin_addr.s_addr);
    *port = ntohs(reinterpret_cast<struct sockaddr_in *>(&clientaddr)->sin_port);
    if (acceptorSocket == UDT::INVALID_SOCK)
    {
        std::cerr << "StartAcceptor UDT::accept error " << UDT::getlasterror().getErrorMessage() << "\n";
        return UDT::INVALID_SOCK;
    }
    return acceptorSocket;
}

UDTSOCKET UDTWrapper::StartClient(const std::string &serverAddress, const std::string &port, uint32_t *localIPAddress)
{
    addrinfo addrinfoHints = {};
    addrinfo *addrinfoResult;

    addrinfoHints.ai_flags = AI_PASSIVE;
    addrinfoHints.ai_family = AF_INET;
    addrinfoHints.ai_socktype = SOCK_STREAM;
    //hints.ai_socktype = SOCK_DGRAM;

    UDTSOCKET clientSocket;
    try
    {
        if (getaddrinfo(NULL, port.c_str(), &addrinfoHints, &addrinfoResult))
            throw("StartClient getaddrinfo error: illegal address, port number or port is busy"s);
        struct sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *)addrinfoResult->ai_addr;
        *localIPAddress = ntohl(sockaddr_ipv4->sin_addr.s_addr);

        clientSocket = UDT::socket(addrinfoResult->ai_family, addrinfoResult->ai_socktype, addrinfoResult->ai_protocol);
        if (clientSocket == UDT::INVALID_SOCK)
            throw("StartClient UDT::socket error"s + UDT::getlasterror().getErrorMessage());

        // UDT Options
        int sndBuf = 10000000;
        int rcvBuf = 10000000;
        bool useRendezvous = true;
        bool resuseAddr = true;
        UDT::setsockopt(clientSocket, 0, UDT_RCVBUF, &sndBuf, sizeof(int));
        UDT::setsockopt(clientSocket, 0, UDP_RCVBUF, &rcvBuf, sizeof(int));
        UDT::setsockopt(clientSocket, 0, UDT_REUSEADDR, &resuseAddr, sizeof(bool));

        // Windows UDP issue
        // For better performance, modify HKLM\System\CurrentControlSet\Services\Afd\Parameters\FastSendDatagramThreshold
#ifdef WIN32
        UDT::setsockopt(clientSocket, 0, UDT_MSS, new int(1052), sizeof(int));
#endif
        if (useRendezvous)
        {
            UDT::setsockopt(clientSocket, 0, UDT_RENDEZVOUS, new bool(true), sizeof(bool));
            if (UDT::ERROR == UDT::bind(clientSocket, addrinfoResult->ai_addr, int(addrinfoResult->ai_addrlen)))
                throw("StartServer UDT::bind error"s + UDT::getlasterror().getErrorMessage());
        }
    }
    catch (std::string errorMessage)
    {
        std::cerr << errorMessage;
        if (clientSocket != UDT::INVALID_SOCK) UDT::close(clientSocket);
        freeaddrinfo(addrinfoResult);
        return UDT::INVALID_SOCK;
    }
    freeaddrinfo(addrinfoResult);

    addrinfo *addrinfoResult2;
    try
    {
         if (getaddrinfo(serverAddress.c_str(), port.c_str(), &addrinfoHints, &addrinfoResult2))
            throw ("StartClient getaddrinfo error: illegal address, port number or port is busy"s);

        if (UDT::connect(clientSocket, addrinfoResult2->ai_addr, int(addrinfoResult2->ai_addrlen)) == UDT::ERROR)
            throw("StartServer UDT::connect error"s + UDT::getlasterror().getErrorMessage());
    }
    catch (std::string errorMessage)
    {
        std::cerr << errorMessage;
        if (clientSocket != UDT::INVALID_SOCK) UDT::close(clientSocket);
        freeaddrinfo(addrinfoResult2);
        return UDT::INVALID_SOCK;
    }

    freeaddrinfo(addrinfoResult2);
    return clientSocket;
}


// return the number of bytes actually sent
int UDTWrapper::SendData(UDTSOCKET sendSocket, char *data, int numBytes)
{
    int total = 0; // how many bytes we have sent
    int bytesleft = numBytes; // how many we have left to send
    int n;
    while(total < numBytes)
    {
        n = UDT::send(sendSocket, data + total, bytesleft, 0);
        if (n == UDT::ERROR) return UDT::ERROR;
        total += n;
        bytesleft -= n;
    }

    return total;
}


// returns number of bytes read
// or UDT::ERROR on error
// set timeout to 0 if want immediate response
int UDTWrapper::ReceiveData(UDTSOCKET receiveSocket, char *data, int numBytes, long secTimeout, long usecTimeout)
{
    int total = 0; // how many bytes we have sent
    int bytesleft = numBytes; // how many we have left to send
    int n;
    while(total < numBytes)
    {
        if (CheckReceiver(receiveSocket, secTimeout, usecTimeout) != 1) return UDT::ERROR;
        n = UDT::recv(receiveSocket, data + total, bytesleft, 0);
        if (n == UDT::ERROR) return UDT::ERROR;
        if (n <= 0) break;
        total += n;
        bytesleft -= n;
    }
    return total;
}

// check whether there is anything waiting to be received
// set timeout to 0 if want immediate response
int UDTWrapper::CheckReceiver(UDTSOCKET receiveSocket, long secTimeout, long usecTimeout)
{
    UDT::UDSET readfds;
    struct timeval tv;

    // set up the file descriptor set
    UD_ZERO(&readfds);
    UD_SET(receiveSocket, &readfds);

    // set up the struct timeval for the timeout
    tv.tv_sec = secTimeout;
    tv.tv_usec = usecTimeout;

    // wait until timeout or data received
    // note 1st parameter is ignored on Windows
    int res = UDT::select(0, &readfds, nullptr, nullptr, &tv);

    // 0 if nothing ready, 1 if something there, -1 on error
    if (res == UDT::ERROR) return -1;
    if (UD_ISSET(receiveSocket, &readfds)) return 1;
    return 0;
}

