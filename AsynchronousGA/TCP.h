/*
 *  TCP.h
 *  GeneralGA
 *
 *  Created by Bill Sellers on 05/04/2008.
 *  Copyright 2008 Bill Sellers. All rights reserved.
 *
 */

#ifndef TCP_H
#define TCP_H

#include <sys/types.h> 
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

namespace AsynchronousGA
{

#ifdef _WIN32
#define SOCKET_TYPE SOCKET
#else
#define SOCKET_TYPE int
#endif

class Genome;
class TCP;

class TCP
{
public:
    TCP();
    ~TCP();

    int StartServer(int MYPORT);
    void StopServer();
    int StartClient(int port, const char *serverAddress);
    void StopClient();
    int StartAcceptor(SOCKET_TYPE listeningSocket);
    void StopAcceptor();
    int ReceiveData(char *data, int numBytes, long secTimeout, long usecTimeout);
    int SendData(char *data, int numBytes);
    int CheckAlive(struct sockaddr_in * /* destination */) { return 0; }
    int CheckReceiver(long secTimeout, long usecTimeout);

    struct sockaddr_in  *GetMyAddress() { return &m_myAddress; }
    struct sockaddr_in  *GetSenderAddress() { return &m_senderAddress; }
    SOCKET_TYPE GetSocket() { return m_Sockfd; }
    bool GetConstructorFailed() { return m_constructorFailed; }

protected:

    SOCKET_TYPE m_Sockfd;
    struct sockaddr_in m_myAddress; // my address information
    struct sockaddr_in m_senderAddress; // my address information
    bool m_serverStarted; // flag for clear up
    bool m_clientStarted; // flag for clear up
    bool m_acceptorStarted; // flag for clear up
    bool m_constructorFailed; // used to check for errors
};

}
#endif
