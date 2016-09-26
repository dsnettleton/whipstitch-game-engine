/**
 *  wsNetworkManager.cpp
 *  Sep 22, 2016
 *  D. Scott Nettleton
 *
 *  This file implements the class wsNetworkManager, which contains all the logic
 *  and subclasses necessary for networking between servers, clients, and P2P.
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2016
 */

#include "wsNetworkManager.h"

wsNetworkManager wsNetworking;

void wsNetworkManager::startUp() {
  _mInitialized = true;
  socket = wsNew(wsNetworkSocket, wsNetworkSocket());
}// End Engine Subsystem startup method

void wsNetworkManager::updateSockets() {
	socket.receivePackets();
}//	End public method updateSockets

void wsNetworkManager::shutDown() {
}// End Engine Subsystem shutdown method
