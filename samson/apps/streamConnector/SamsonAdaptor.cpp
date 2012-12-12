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
#include "StreamConnector.h"

namespace stream_connector {
SamsonConnection::SamsonConnection(Adaptor *_item
                                   , ConnectionType _type
                                   , std::string host
                                   , int port
                                   , std::string queue
                                   )
  : Connection(_item, _type, au::str("SAMSON(%s:%d:%s)", host.c_str(), port, queue.c_str())) {
  // No init client by default here
  client_ = NULL;

  // Keep information for connection
  host_ = host;
  port_ = port;
  queue_ = queue;

  // Init values
  num_connection_trials = 0;
}

void SamsonConnection::try_connect() {
  if ((client_) && (client_->connection_ready())) {
    return;
  }

  if (!client_) {
    client_ = new samson::SamsonClient("connector");
    client_->set_receiver_interface(this);                    // Set me as the receiver of live data from SAMSON
  }

  // Update counter to trials
  num_connection_trials++;
  cronometer_reconnection.Reset();

  // Try to reconnect
  if (client_->connect(au::str("%s:%d", host_.c_str(), port_))) {
    // Note: At the moment, it is not possible to specify flags new of clear here
    if (getType() == connection_input) {
      client_->connect_to_queue(queue_, false, false);
    }
  }
}

SamsonConnection::~SamsonConnection() {
  if (client_) {
    delete client_;
  }
}

void SamsonConnection::start_connection() {
  // Try to connect by the first time
  try_connect();
}

void SamsonConnection::stop_connection() {
  // TODO: Stop all theads since it will be removed
}

void SamsonConnection::review_connection() {
  // Nothing to do here
  if (!client_) {
    set_as_connected(false);
    if (cronometer_reconnection.seconds() < 3) {
      return;           // Do not try to connect again...
    }
    // Try to reconnect here...
    try_connect();
    return;
  }

  set_as_connected(client_->connection_ready());
}

size_t SamsonConnection::bufferedSizeOnMemory() {
  if (getType() == connection_output) {
    return Connection::bufferedSize() + client_->GetPendingSizeToPush();
  } else {
    return 0;
  }
}

size_t SamsonConnection::bufferedSize() {
  if (getType() == connection_output) {
    return Connection::bufferedSize() + client_->GetPendingSizeToPush();
  } else {
    return 0;
  }
}

// Overload method to push blocks using samsonClient
void SamsonConnection::push(engine::BufferPointer buffer) {
  if (getType() == connection_input) {
    return;         // Nothing to do if we are input
  }
  // Report manually size ( not we are overloading Connection class )
  report_output_size(buffer->size());

  // Push this block directly to the SAMSON client
  client_->push(buffer, queue_);
}

// Overwriteen method of SamsonClient
void SamsonConnection::receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer) {
  // Transformation of buffer
  samson::KVHeader *header = (samson::KVHeader *)buffer->data();

  if (header->IsTxt()) {
    // Remove header of the received block
    size_t real_size = buffer->size() - sizeof(samson::KVHeader);
    engine::BufferPointer buffer2 = engine::Buffer::Create("TXT " + buffer->name(), real_size);
    buffer2->set_size(real_size);
    memcpy(buffer2->data(), buffer->data() + sizeof(samson::KVHeader), real_size);

    // Push the new buffer
    pushInputBuffer(buffer2);
  } else {
    LOG_SW(("Received a binary buffer %s from %s. Still not implemented how to process this"
            , au::str(buffer->size(), "B").c_str(), getFullName().c_str()));
  }
}

std::string SamsonConnection::getStatus() {
  if (!client_) {
    return "Not connected";
  }

  if (client_->connection_ready()) {
    return "Connected";
  } else {
    return "Trying to connect...";
  }
}

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
  add(new SamsonConnection(this, getType(), host, port, queue));
}

void SamsonAdaptor::review_item() {
}
}

