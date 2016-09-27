/**
 *  wsNetworkSocket.h
 *  Sep 22, 2016
 *  D. Scott Nettleton
 *
 *  This file declares the class wsNetworkSocket, which is used to send
 *  UDP packets over the network
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2016
*/
#ifndef WS_NETWORK_SOCKET_H_
#define WS_NETWORK_SOCKET_H_

#define WS_BACKEND_SOCKETS 0x0001

#define WS_NETWORKING_BACKEND WS_BACKEND_SOCKETS

#include "../wsUtils.h"

#if WS_NETWORKING_BACKEND == WS_BACKEND_SOCKETS
  #ifdef WS_OS_FAMILY_WINDOWS
    #include <winsock2.h>
    #pragma comment( lib, "wsock32.lib" )
  #else //  Unix Family
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
  #endif
#endif

struct wsNetworkAddress {
  u8 a = 127;
  u8 b = 0;
  u8 c = 0;
  u8 d = 1;
  u32 compact() { return ((u32)a << 24) | ((u32)b << 16) | ((u32)c << 8) | (u32)d; }
};

class wsNetworkSocket {
  private:
    i32 socketHandle;
    sockaddr_in address;
  public:
    //  Constructor and Destructor
    wsNetworkSocket();
    ~wsNetworkSocket();
    //  Setters and Getters
    void setAddress(wsNetworkAddress* ipAddress, u16 port = WS_UDP_PORT);
    //  Operational Methods
    void receivePackets();
    void sendPacket(const char* packetData, i32 packetLength);
};//  End class wsNetworkSocket

#endif //  WS_NETWORK_SOCKET_H_
