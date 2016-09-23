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
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef WS_NETWORK_SOCKET_H_
#define WS_NETWORK_SOCKET_H_

#define WS_BACKEND_SOCKETS 0x0001
#define WS_BACKEND_ASIO    0x0002

#define WS_NETWORKING_BACKEND WS_BACKEND_SOCKETS

#include "../wsUtils.h"

#if WS_NETWORKING_BACKEND == WS_BACKEND_ASIO
  #include "asio.h"
#elif WS_NETWORKING_BACKEND == WS_BACKEND_SOCKETS
  #ifdef WS_OS_FAMILY_WINDOWS
    #include <winsock2.h>
    #pragma comment( lib, "wsock32.lib" )
  #else //  Unix Family
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
  #endif
#endif

class wsNetworkSocket {
  private:
    i32 socketHandle;
    sockaddr_in address;
  public:
    //  Constructor and Destructor
    wsNetworkSocket();
    ~wsNetworkSocket();
    //  Setters and Getters
    //  Operational Methods
    void sendPacket(const char* packetData, i32 packetLength);
};//  End class wsNetworkSocket

#endif //  WS_NETWORK_SOCKET_H_
