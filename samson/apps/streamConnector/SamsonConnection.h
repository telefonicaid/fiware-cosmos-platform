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
#ifndef _H_STREAM_CONNECTOR_SAMSON_CONNECTION
#define _H_STREAM_CONNECTOR_SAMSON_CONNECTION

#include "au/mutex/Token.h"

#include "Adaptor.h"
#include "Connection.h"
#include "common.h"

namespace stream_connector {
class Block;

class SamsonConnection : public Connection, public samson::DelilahLiveDataReceiverInterface {
public:


  SamsonConnection(Adaptor *_item
                   , ConnectionType _type
                   , std::string host
                   , int port
                   , std::string queue
                   );


  ~SamsonConnection();

  virtual void start_connection();
  virtual void stop_connection();
  virtual void review_connection();

  virtual size_t bufferedSize() const;
  virtual size_t bufferedSizeOnMemory() const;

  void push(engine::BufferPointer buffer);

  // Overwriteen method of SamsonClient
  void ReceiveBufferFromQueue(const std::string& queue, engine::BufferPointer buffer);

  std::string getStatus();

private:

  void try_connect();

  // Information to stablish the connection with the SAMSON system
  std::string host_;
  int port_;
  std::string queue_;

  samson::SamsonClient *client_;

  // Connections trials information
  int num_connection_trials;
  au::Cronometer cronometer_reconnection;
};
}

#endif  // ifndef _H_STREAM_CONNECTOR_STREAM_CONNECTION

