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

wsNetworkConnection::wsNetworkConnection(u32 _protocolID, t64 _timeout) : protocolID(_protocolID), timeout(_timeout) {
}// End wsNetworkConnection Constructor
