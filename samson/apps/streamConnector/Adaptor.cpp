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


#include "au/singleton/Singleton.h"

#include "Adaptor.h"        // Own interface
#include "Channel.h"
#include "Connection.h"

namespace stream_connector {
Adaptor::Adaptor (Channel *channel, ConnectionType type, std::string description)
  : token_("connector::Item") {
  channel_ = channel;
  type_ = type;
  description_ = description;

  next_id_ = 0;

  canceled_ = false;       // Default value
  finished_ = false;
}

Adaptor::~Adaptor() {
  // Cancel this item to make sure we do not remove with any thread running here
  cancel_item();

  // Remover all connections
  connections_.clearMap();
}

ConnectionType Adaptor::type() const {
  return type_;
}

std::string Adaptor::name() const {
  return name_;
}

std::string Adaptor::fullname() const {
  return au::str("%s.%s", channel_->getName().c_str(), name_.c_str());
}

std::string Adaptor::description() const {
  return description_;
}

const char *Adaptor::GetTypeStr() const {
  return str_ConnectionType(type_);
}

void Adaptor::add(Connection *connection) {
  au::TokenTaker tt(&token_);

  connections_.insertInMap(next_id_, connection);
  connection->id_ = next_id_;

  LOG_SV(("Connection %s (%s) added"
          , connection->fullname().c_str()
          , connection->description().c_str()));

  // Init the connection properly
  connection->init_connecton();

  next_id_++;
}

void Adaptor::push(engine::BufferPointer buffer) {
  au::TokenTaker tt(&token_);

  au::map<int, Connection>::iterator it_connections;
  for (it_connections = connections_.begin(); it_connections != connections_.end(); ++it_connections) {
    Connection *connection = it_connections->second;
    if (connection->type() == connection_output) {
      connection->push(buffer);
    }
  }
}

void Adaptor::review() {
  if (canceled_) {
    return;         // Not call review
  }
  if (!finished_) {
    review_item();  // Review all connections
  }
  {
    au::TokenTaker tt(&token_);
    au::map<int, Connection>::iterator it_connections;

    for (it_connections = connections_.begin(); it_connections != connections_.end(); ++it_connections) {
      Connection *connection = it_connections->second;
      connection->review();
    }
  }
}

void Adaptor::init_item() {
  start_item();
};
void Adaptor::cancel_item() {
  canceled_ = true;       // This will block future calls to review

  stop_item();
  {
    // Mutex protectio
    au::TokenTaker tt(&token_);

    // Cancel all connections
    au::map<int, Connection>::iterator it_connections;
    for (it_connections = connections_.begin(); it_connections != connections_.end(); ++it_connections) {
      Connection *connection = it_connections->second;
      connection->cancel_connecton();
    }
  }
}

int Adaptor::num_connections() const {
  au::TokenTaker tt(&token_);

  return connections_.size();
}

void Adaptor::set_as_finished() {
  if (finished_) {
    return;
  }
  finished_ = true;
}

bool Adaptor::is_finished() {
  return finished_;
}

size_t Adaptor::GetConnectionsBufferedSize() const {
  au::TokenTaker tt(&token_);
  size_t total = 0;

  au::map<int, Connection>::const_iterator it_connections;
  for (it_connections = connections_.begin(); it_connections != connections_.end(); ++it_connections) {
    Connection *connection = it_connections->second;
    total += connection->bufferedSize();
  }

  return total;
}

void Adaptor::remove_finished_connections(au::ErrorManager *error) {
  // Mutex protectio
  au::TokenTaker tt(&token_);

  // Cancel all connections
  au::map<int, Connection>::iterator it_connections;
  for (it_connections = connections_.begin(); it_connections != connections_.end(); ++it_connections) {
    Connection *connection = it_connections->second;
    if (connection->is_finished()) {
      connection->cancel_connecton();
      delete connection;
      connections_.erase(it_connections);
    }
  }
}

void Adaptor::report_output_size(size_t size) {
  traffic_statistics_.push_output(size);
  channel_->report_output_size(size);
}

void Adaptor::report_input_size(size_t size) {
  traffic_statistics_.push_input(size);
  channel_->report_input_size(size);
}

bool Adaptor::accept(InputInterChannelConnection *connection) {
  // By default, we do not accept interchannel connections
  // Only specific items accept this type of connection
  return false;
}

Connection *Adaptor::getFirstConnection() {
  if (connections_.size() == 0) {
    return NULL;
  }
  return connections_.begin()->second;
}
}