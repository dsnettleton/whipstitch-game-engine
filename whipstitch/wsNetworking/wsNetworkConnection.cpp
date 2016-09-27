/**
 *  wsNetworkConnection.cpp
 *  Sep 26, 2016
 *  D. Scott Nettleton
 *
 *  This file implements the class wsNetworkConnection, which is used handle connections
 *  between clients, servers or P2P.
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2016
 */

#include "wsNetworkConnection.h"

//  Constructor and Destructor  ////////////////////////////////////////////////

wsNetworkConnection::wsNetworkConnection(u32 _protocolID = WS_PROTOCOL_ID, t64 _timeout = 15.0)
    : protocolID(_protocolID), timeout(_timeout) {
  socket = wsNew(wsNetworkSocket, wsNetworkSocket());
}// End wsNetworkConnection Constructor

void wsNetworkConnection::connect(wsNetworkAddress* ipAddress, u16 port = WS_UDP_PORT) {
  wsEcho(WS_LOG_NETWORKING, "Connecting to %d.%d.%d.%d:%d", ipAddress->a, ipAddress->b, ipAddress->c, ipAddress->d, port);
  reset();
  socket.setAddress(ipAddress, port);
  currentMode = _ws_connection_mode.CLIENT;
  currentState = _ws_connection_state.CONNECTING;
}// End public method connect

void wsNetworkConnection::listen() {
  currentState = _ws_connection_state.LISTENING;
}// End private method listen

bool wsNetworkConnection::receivePacket(wsByte data[]) {
  return false;
}// End public method receivePacket

void wsNetworkConnection::reset() {
  currentState = _ws_connection_state.DISCONNECTED;
  currentMode = _ws_connection_mode.NONE;
  inactiveTime = 0.0;
}// End public method reset

void wsNetworkConnection::update(t64 deltaTime) {
  inactiveTime += deltaTime;
  if (inactiveTime >= timeout) {
    wsEcho(WS_LOG_NETWORKING, "Connection Timed Out.");
    reset();
    return;
  }
  if (currentMode == _ws_connection_mode.SERVER || currentMode == _ws_connection_mode.P2P) {
    listen();
  }// End if we should listen for incoming connections
}// End public method update
