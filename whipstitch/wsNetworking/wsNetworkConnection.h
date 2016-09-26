/**
 *  wsNetworkConnection.h
 *  Sep 26, 2016
 *  D. Scott Nettleton
 *
 *  This file declares the class wsNetworkConnection, which is used handle connections
 *	between clients, servers or P2P.
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2016
 */

#ifndef WS_NETWORK_CONNECTION_H_
#define WS_NETWORK_CONNECTION_H_

#include "wsNetworkSocket.h"

enum wsConnectionMode {
  None,
  Client,
  Server,
  P2P
};

enum wsConnectionState {
  Disconnected,
  Connecting,
  Connected,
  Listening,
  Error
};

class wsNetworkConnection {
  private:
  	u32 protocolID;
  	t64 timeout;
    wsNetworkSocket* socket;
    wsConnectionMode mode;
    wsConnectionState state;
    t64 inactiveTime;
  public:
  	//	Constructor and Destructor
  	wsNetworkConnection(u32 _protocolID, t64 _timeout);
  	~wsNetworkConnection();
  	//	Setters and Getters
  	//	Operational Methods
  	void receivePacket();
    void reset();
  	void sendPacket();
  	void start();
  	void stop();
  	void update();
};//	End class wsNetworkConnection

#endif	//	WS_NETWORK_CONNECTION_H_
