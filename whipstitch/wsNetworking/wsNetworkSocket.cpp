/**
 *  wsNetworkSocket.cpp
 *  Sep 22, 2016
 *  D. Scott Nettleton
 *
 *  This file implements the class wsNetworkSocket, which is used to send
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

#include "wsNetworkSocket.h"

wsNetworkSocket::wsNetworkSocket() {
  #ifdef WS_OS_FAMILY_WINDOWS
    //  Initialize sockets
    WSADATA WsaData;
    if (WSAStartup(MAKEWORD(2, 2), &WsaData) != NO_ERROR) {
      wsEcho(WS_LOG_ERROR, "Error initializing network sockets.");
    }
  #endif
  //  socket(int domain, int type, int protocol)
  socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  wsAssert(socketHandle > 0, "Failed to create socket.");
  //  Bind the UDP socket to our engine's port number
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons((u16)WS_UDP_PORT);
  wsAssert(bind(socketHandle, (const sockaddr*) &address, sizeof(sockaddr_in)) >= 0, "Failed to bind socket");
  //  Set our socket to non-blocking mode for asynchronous operation
  #ifdef WS_OS_FAMILY_UNIX
    i32 nonBlocking = 1;
    wsAssert(fcntl(socketHandle, F_SETFL, O_NONBLOCK, nonBlocking) != -1, "Failed to set non-blocking");
  #else
    DWORD nonBlocking = 1;
    wsAssert(ioctlsocket(socketHandle, FIONBIO, &nonBlocking) == 0, "Failed to set non-blocking");
  #endif
}// End Constructor

wsNetworkSocket::~wsNetworkSocket() {
  #ifdef WS_OS_FAMILY_WINDOWS
    WSACleanup();
  #endif
}// End Destructor

void wsNetworkSocket::sendPacket(const char* packetData, i32 packetLength) {
  i32 bytesSent = sendto(socketHandle, packetData, packetLength, 0, (sockaddr*)&address, sizeof(sockaddr_in));
  if (bytesSent != packetLength) {
    wsEcho(WS_LOG_NETWORKING, "Failed to send packet");
  }
}// End public method sendPacket
