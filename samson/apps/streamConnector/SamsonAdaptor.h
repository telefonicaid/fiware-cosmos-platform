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
#ifndef _H_STREAM_CONNECTOR_STREAM_CONNECTION
#define _H_STREAM_CONNECTOR_STREAM_CONNECTION

#include "au/mutex/Token.h"

#include "Adaptor.h"
#include "Connection.h"
#include "common.h"

namespace stream_connector {
class Block;

class SamsonAdaptor : public Adaptor {
public:

  SamsonAdaptor(Channel *_channel,
                ConnectionType _type,
                std::string _host,
                int _port,
                std::string _queue);

  std::string getStatus();
  virtual void start_item();
  virtual void review_item();

private:

  // Information to stablish the connection with the SAMSON system
  std::string host;
  int port;
  std::string queue;
};
}

#endif  // ifndef _H_STREAM_CONNECTOR_STREAM_CONNECTION

