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

#ifndef _H_STREAM_CONNECTOR_LISTENER
#define _H_STREAM_CONNECTOR_LISTENER

#include "Adaptor.h"
#include "LogManager.h"
#include "au/network/NetworkListener.h"
#include "common.h"

namespace stream_connector {
class ListenerAdaptor : public Adaptor, public au::NetworkListener, public au::NetworkListenerInterface {
  // Port to open to accept connections
  int port;

  au::Status status_init;

public:

  ListenerAdaptor(Channel *channel, ConnectionType type, int _port);

  // samson::NetworkListenerInterface
  void newSocketConnection(au::NetworkListener *listener
                           , au::SocketConnection *socket_connetion);

  // Get status of this element
  std::string getStatus();

  // Review item: open port if it was not possible in the past...
  void review_item();
  void stop_item();
};
}


#endif  // ifndef _H_STREAM_CONNECTOR_LISTENER
