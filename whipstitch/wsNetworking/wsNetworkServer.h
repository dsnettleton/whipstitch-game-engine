/**
 *  wsNetworkServer.h
 *  Sep 27, 2016
 *  D. Scott Nettleton
 *
 *  This file declares the class wsNetworkServer, which is used to
 *  manage connections between this host and multiple clients.
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2016
*/

#ifndef WS_NETWORK_SERVER_H_
#define WS_NETWORK_SERVER_H_

#include "wsNetworkConnection.h"

class wsNetworkServer {
  private:
    //  Private Members
  public:
    //  Constructor and Destructor
    wsNetworkServer();
    ~wsNetworkServer();
    //  Setters and Getters
    //  Operational Methods
}//  End class wsNetworkServer

#endif  //  WS_NETWORK_SERVER_H_
