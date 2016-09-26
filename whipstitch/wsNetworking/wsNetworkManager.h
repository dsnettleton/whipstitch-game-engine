/**
 *  wsNetworkManager.h
 *  Sep 22, 2016
 *  D. Scott Nettleton
 *
 *  This file declares the class wsNetworkManager, which contains all the logic
 *  and subclasses necessary for networking between servers, clients, and P2P.
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2013-2016
 */

#ifndef WS_NETWORK_MANAGER_H_
#define WS_NETWORK_MANAGER_H_

#include "../wsUtils.h"
#include "wsNetworkSocket.h"

class wsNetworkManager {
  private:
    wsNetworkSocket* socket;
    bool _mInitialized;
  public:
    /*  Default Constructor and Deconstructor */
    //  As an engine subsystem, the network manager takes no action until explicitly
    //  initialized via the startUp(...) function.
    //  uninitialized via the shutDown() function.
    wsNetworkManager() : _mInitialized(false) {}
    ~wsNetworkManager() {}
    void startUp();
    void shutDown();
    //  Setters and Getters
    //  Operational Methods
    void updateSockets();
};// End class wsNetworkManager

extern wsNetworkManager wsNetworking;

#endif  //  WS_NETWORK_MANAGER_H_
