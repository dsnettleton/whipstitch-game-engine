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
    void sendPacket(const char* packetData, i32 packetSize);
};// End class wsNetworkManager

extern wsNetworkManager wsNetworking;

#endif  //  WS_NETWORK_MANAGER_H_
