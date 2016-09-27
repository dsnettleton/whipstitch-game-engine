/**
 *  wsNetworkConnection.h
 *  Sep 26, 2016
 *  D. Scott Nettleton
 *
 *  This file declares the class wsNetworkConnection, which is used handle connections
 *  between clients, servers or P2P.
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2016
 */

#ifndef WS_NETWORK_CONNECTION_H_
#define WS_NETWORK_CONNECTION_H_

#include "wsNetworkSocket.h"

//  Define a unique protocol ID used to filter packets.
//  All packets will have this value as their first four bytes.
#define WS_PROTOCOL_ID 0x47616D65 //  Game

class wsNetworkConnection {
  private:
    enum _ws_connection_mode {
      NONE,
      CLIENT,
      SERVER,
      P2P
    };

    enum _ws_connection_state {
      DISCONNECTED,
      CONNECTING,
      CONNECTED,
      LISTENING,
      ERROR
    };
    u32 protocolID;
    t64 timeout;
    wsNetworkSocket* socket;
    _ws_connection_mode currentMode;
    _ws_connection_state currentState;
    t64 inactiveTime;
    //  Private methods
    void listen();
  public:
    //  Constructor and Destructor
    wsNetworkConnection(u32 _protocolID = WS_PROTOCOL_ID, t64 _timeout = 15.0f);
    ~wsNetworkConnection();
    //  Setters and Getters
    //  Operational Methods
    void connect(wsNetworkAddress* ipAddress, u16 port = WS_UDP_PORT);
    bool receivePacket(wsByte data[]);
    void reset();
    void sendPacket();
    void start();
    void stop();
    void update(t64 deltaTime);
};//  End class wsNetworkConnection

#endif  //  WS_NETWORK_CONNECTION_H_
