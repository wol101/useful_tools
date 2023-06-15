#ifndef UDTWRAPPER_H
#define UDTWRAPPER_H

#include "udt4/udt.h"

class UDTWrapper
{
public:

    static UDTSOCKET StartServer(const std::string &port);
    static UDTSOCKET StartAcceptor(UDTSOCKET serverSocket, uint32_t *address, uint16_t *port);
    static UDTSOCKET StartClient(const std::string &serverAddress, const std::string &port, uint32_t *localIPAddress);

    static int ReceiveData(UDTSOCKET receiveSocket, char *data, int numBytes, long secTimeout, long usecTimeout);
    static int CheckReceiver(UDTSOCKET receiveSocket, long secTimeout, long usecTimeout);
    static int SendData(UDTSOCKET sendSocket, char *data, int numBytes);

};

class UDTUpDown
{
public:
   UDTUpDown()
   {
      // use this function to initialize the UDT library
      m_status = UDT::startup();
   }
   ~UDTUpDown()
   {
      // use this function to release the UDT library
      UDT::cleanup();
   }

   int status() const { return m_status; }

private:
   int m_status = 0;
};

class UDTCloseSocketGuard
{
public:
    UDTCloseSocketGuard(UDTSOCKET socket) { m_socket = socket; }
    ~UDTCloseSocketGuard() { UDT::close(m_socket); }
private:
    UDTSOCKET m_socket;
};


#endif // UDTWRAPPER_H
