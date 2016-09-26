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
*/

#include "wsNetworkSocket.h"

//  CONSTRUCTOR & DESTRUCTOR  /////////////////////////////////////////

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
    closesocket(socketHandle);
    WSACleanup();
  #else
    close(socketHandle);
  #endif
}// End Destructor

//  SETTERS AND GETTERS     ///////////////////////////////////////////

void wsNetworkSocket::setAddress(u8 a = 127, u8 b = 0, u8 c = 0, u8 d = 1, u16 port = WS_UDP_PORT) {
  u32 newAddress = ((u32)a << 24) | ((u32)b << 16) | ((u32)c << 8) | (u32)d;
  address.sin_addr.s_addr = htonl(newAddress);
  address.sin_port = htons(port);
}// End public method setAddress

//  OPERATIONAL METHODS     ///////////////////////////////////////////

void wsNetworkSocket::receivePackets() {
  u8 packetData[WS_MAX_PACKET_SIZE];
  u32 maxPacketSize = sizeof(packetData);
  #ifdef WS_OS_FAMILY_WINDOWS
    typedef i32 socklen_t;
  #endif
  sockaddr_in from;
  socklen_t fromLength = sizeof(from);
  i32 bytes = recvfrom(socketHandle, (u8*)packetData, maxPacketSize, 0, (sockaddr*)&from, &fromLength);
  if (bytes <= 0) {
    return;
  }
  u32 fromAddr = ntohl(from.sin_addr.s_addr);
  u16 fromPort = ntohs(from.sin_port);
  //  Process Received Packet
}// End public method receivePackets

void wsNetworkSocket::sendPacket(const char* packetData, i32 packetLength) {
  i32 bytesSent = sendto(socketHandle, packetData, packetLength, 0, (sockaddr*)&address, sizeof(sockaddr_in));
  if (bytesSent != packetLength) {
    wsEcho(WS_LOG_NETWORKING, "Failed to send packet");
  }
}// End public method sendPacket
