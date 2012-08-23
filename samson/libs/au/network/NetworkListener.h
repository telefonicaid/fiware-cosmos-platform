
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

class NetworkListenerInterface {
public:
  virtual void newSocketConnection(NetworkListener *listener,
                                   SocketConnection *socket_connetion) = 0;
};


class NetworkListener {
public:

  NetworkListener(NetworkListenerInterface *_network_listener_interface);
  ~NetworkListener();

  // Init and close this listener
  Status InitNetworkListener(int port);
  void StopNetworkListener();
  bool IsNetworkListenerRunning() const;

  // Accesorts
  int port() const;

  // Return code of the background thread ( only used in test )
  void *background_thread_return_code();

private:

  // Main function to run in background
  void *runNetworkListener();

  SocketConnection *acceptNewNetworkConnection(void);

  // Delegate to notify about new connections
  NetworkListenerInterface *network_listener_interface_;

  // Port where we are listening
  int port_;

  // Internal file descriptor
  int rFd_;

  // Background thread
  pthread_t t;

  // Flag indicating if the background thread is running ( to joint at destructor )
  bool background_thread_running_;
  bool background_thread_finished_;

  // Code returned by bacground thread
  void *return_code_;

  // Background thread function
  friend void *NetworkListener_run(void *p);
};
}

#endif  // ifndef _H_AU_NETWORK_LISTENER
