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

#include "FileDescriptorConnection.h"
#include "ListenerAdaptor.h"  // Own interfave
#include "StreamConnector.h"

namespace stream_connector {
ListenerAdaptor::ListenerAdaptor(Channel *channel, ConnectionType type, int _port) :
  Adaptor(channel, type, au::str("LISTEN(%d)", _port)),
  au::NetworkListener(this) {
  // Keep the port
  port = _port;

  // Init listener and run in background
  status_init = InitNetworkListener(port);
}

// samson::NetworkListenerInterface
void newSocketConnection(au::NetworkListener *listener
                         , au::SocketConnection *socket_connection);

// Get status of this element
std::string ListenerAdaptor::getStatus() {
  if (IsNetworkListenerRunning()) {
    return "Listening";
  } else {
    return au::str("NOT Listening (%s)", au::status(status_init));
  }
}

// Review item: open port if it was not possible in the past...
void ListenerAdaptor::review_item() {
  if (!IsNetworkListenerRunning()) {
    // Init again
    status_init = InitNetworkListener(port);
  }
}

void ListenerAdaptor::stop_item() {
  // Stop listener
  if (IsNetworkListenerRunning()) {
    StopNetworkListener();
  }
}

void ListenerAdaptor::newSocketConnection(au::NetworkListener *listener
                                          , au::SocketConnection *socket_connection) {
  std::string name  = au::str("Socket %s", socket_connection->host_and_port().c_str());

  FileDescriptorConnection *new_connection = new SimpleFileDescriptorConnection(this
                                                                                , type()
                                                                                , name
                                                                                , socket_connection);

  // Add this item as my children item
  add(new_connection);
}
}
