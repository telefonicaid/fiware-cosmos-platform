/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#ifndef _H_AU_NETWORK_LISTENER
#define _H_AU_NETWORK_LISTENER

#include "au/Status.h"
#include "au/Thread.h"

/*****************************************************************************
*
*  NetworkListener
*
*  Handy class to open a port and accept connections
*  A SocketConnection instance is created for each new connection
*  Callback interface implemented using NetworkListenerInterface
*
*****************************************************************************/

namespace au {
class SocketConnection;
class NetworkListener;

/**
 * \brief Interface of NetworkListener to handle incoming connections
 */

class NetworkListenerInterface {
public:
  virtual void newSocketConnection(NetworkListener *listener, SocketConnection *socket_connection) = 0;
};

/**
 * \brief Listener over a particular port. Incomming connections handled with an interface
 */

class NetworkListener : public au::Thread {
public:

  NetworkListener(NetworkListenerInterface *_network_listener_interface);
  virtual ~NetworkListener();

  // Init and close this listener
  Status InitNetworkListener(int port);
  void StopNetworkListener();
  bool IsNetworkListenerRunning() const;

  int port() const;

private:

  /**
   * \brief Main function to run in background
   */
  virtual void RunThread();

  SocketConnection *AcceptNewNetworkConnection(void);

  volatile int rFd_;   /**< Internal file descriptor */
  int port_;   /**< Port where we are listening */
  NetworkListenerInterface *network_listener_interface_;   /**< Delegate to notify about new connections */
};
}

#endif  // ifndef _H_AU_NETWORK_LISTENER
