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

#include "Channel.h"

#include "SamsonAdaptor.h"  // Own interface

#include "SamsonConnection.h"
#include "StreamConnector.h"

namespace stream_connector {
SamsonAdaptor::SamsonAdaptor(
  Channel *_channel
  , ConnectionType _type
  ,
  std::string _host
  , int _port
  , std::string _queue)
  :
    Adaptor(_channel
            , _type
            , au::str("SAMSON(%s:%d)", _host.c_str(), _port)) {
  // Information for connection
  host = _host;
  port = _port;
  queue = _queue;
}

// Get status of this element
std::string SamsonAdaptor::getStatus() {
  return "OK";
}

void SamsonAdaptor::start_item() {
  // Add connection
  add(new SamsonConnection(this, type(), host, port, queue));
}

void SamsonAdaptor::review_item() {
}
}

