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


#include "NetworkConnection.h"  // Own interface

#include "au/ThreadManager.h"
#include "samson/common/Logs.h"
#include "samson/common/MessagesOperations.h"
#include "samson/network/NetworkManager.h"


namespace samson {
void *NetworkConnection_readerThread(void *p) {
  // Free resources automatically when this thread finish
  pthread_detach(pthread_self());

  NetworkConnection *network_connection = ( NetworkConnection * )p;
  network_connection->readerThread();
  network_connection->running_t_read_ = false;
  return NULL;
}

void *NetworkConnection_writerThread(void *p) {
  // Free resources automatically when this thread finish
  pthread_detach(pthread_self());

  NetworkConnection *network_connection = ( NetworkConnection * )p;
  network_connection->writerThread();
  network_connection->running_t_write_ = false;
  return NULL;
}

NetworkConnection::NetworkConnection(const NodeIdentifier& node_identifier
                                     , au::SocketConnection *socket_connection
                                     , NetworkManager *network_manager)
  : token_("NetworkConnection") {
  node_identifier_ = node_identifier;

  // Connection we are managing
  socket_connection_ = socket_connection;

  // Manager to report received messages
  network_manager_ = network_manager;

  // Init flags about threads
  running_t_read_ = false;
  running_t_write_ = false;

  // Create both threads for writing and reading
  if (!running_t_read_) {
    std::string name = au::str("NetworkConnection::initReadWriteThreads::read (%s)"
                               , socket_connection->host_and_port().c_str());

    running_t_read_ = true;
    au::Singleton<au::ThreadManager>::shared()->AddThread(name, &t_read_, NULL, NetworkConnection_readerThread, this);
  }

  if (!running_t_write_) {
    std::string name = au::str("NetworkConnection::initReadWriteThreads::write (%s)"
                               , socket_connection->host_and_port().c_str());

    running_t_write_ = true;
    au::Singleton<au::ThreadManager>::shared()->AddThread(name, &t_write_, NULL, NetworkConnection_writerThread, this);
  }
}

NetworkConnection::~NetworkConnection() {
  // Make sure everything in this class is gone
  CloseAndStopBackgroundThreads();

  if (running_t_read_) {
    LM_X(1, ("Deleting Network connection with a running read thread"));
  }
  if (running_t_write_) {
    LM_X(1, ("Deleting Network connection with a running write thread"));
  }
  delete socket_connection_;
}

// Wake up writer thread
void NetworkConnection::WakeUpWriter() {
  au::TokenTaker tt(&token_);  // Wake up writing thread if necessary

  tt.WakeUpAll();
}

bool NetworkConnection::IsDisconnected() const {
  return socket_connection_->IsClosed();
}

void NetworkConnection::Close() {
  if (!socket_connection_->IsClosed()) {
    LOG_SW(("Closing connection %s", node_identifier_.str().c_str()));
    socket_connection_->Close();
  }
}

void NetworkConnection::CloseAndStopBackgroundThreads() {
  if (!socket_connection_->IsClosed()) {
    LOG_SW(("Closing connection %s", node_identifier_.str().c_str()));
    socket_connection_->Close();
  }

  // Wait until both thread are gone
  au::Cronometer cronometer;
  while (true) {
    if (!running_t_read_ || (pthread_equal(pthread_self(), t_read_))) {
      if (!running_t_write_ || (pthread_equal(pthread_self(), t_write_))) {
        return;
      }
    }

    WakeUpWriter();

    usleep(100000);
    if (cronometer.seconds() > 1) {
      LOG_SW(("Waiting for background threads of connection %s", node_identifier_.str().c_str()));
      cronometer.Reset();
    }
  }
}

void NetworkConnection::readerThread() {
  while (true) {
    if (socket_connection_->IsClosed()) {
      // Wake up writing thread if necessary
      au::TokenTaker tt(&token_);
      tt.WakeUpAll();
      return;
    }

    // Read a packet
    PacketPointer packet(new Packet());

    size_t total_read = 0;

    // Read packet
    au::Status s = packet->read(socket_connection_, &total_read);

    // Monitor rate
    rate_in_.Push(total_read);

    if (s == au::OK) {
      packet->from = node_identifier_;   // Set from "node identifier"
      network_manager_->receive(this, packet);
    } else {
      if (!socket_connection_->IsClosed()) {
        LOG_SW(("Closing connection %s: Error reading %s", node_identifier_.str().c_str(), au::status(s)));
        socket_connection_->Close();   // Close connection since a packet has not been received correctly
      }
    }
  }
}

void NetworkConnection::writerThread() {
  while (true) {
    if (socket_connection_->IsClosed()) {
      return;     // Quit if this connection is closed
    }

    // Get the next packet to be sent by me
    PacketPointer packet = network_manager_->multi_packet_queue_.Front(node_identifier_);

    if (packet != NULL) {
      LOG_V(logs.out_messages, ("Sent packet to %s : %s", packet->to.str().c_str(), packet->str().c_str()));

      size_t total_write = 0;

      // Write the packet over this socket
      au::Status s = packet->write(socket_connection_, &total_write);

      // Monitor rate
      rate_out_.Push(total_write);

      if (s == au::OK) {
        network_manager_->multi_packet_queue_.Pop(node_identifier_);
      } else {
        LOG_SW(("Closing connection %s: Error writing %s", node_identifier_.str().c_str(), au::status(s)));
        socket_connection_->Close();
      }
    } else {
      // Block this thread until new packets are pushed or connection is restablish...
      au::TokenTaker tt(&token_);
      tt.Stop();   // block
    }
  }

  LM_X(1, ("Internal error"));  // No possible to get this line
}

std::string NetworkConnection::str() const {
  std::ostringstream output;

  output << "[" << (running_t_read_ ? "R" : " ") << (running_t_write_ ? "W" : " ") << "]";

  if (socket_connection_->IsClosed()) {
    output << " Disconnected ";
  } else {
    output << " Connected    ";
  } output << "[ " << node_identifier_.str() << " ] ";
  return output.str();
}

size_t NetworkConnection::rate_in() const {
  return rate_in_.rate();
}

size_t NetworkConnection::rate_out() const {
  return rate_out_.rate();
}

NodeIdentifier NetworkConnection::node_identifier() const {
  return node_identifier_;
}

std::string NetworkConnection::host_and_port()  const {
  return socket_connection_->host_and_port();
}

void NetworkConnection::fill(gpb::CollectionRecord *record, const Visualization& visualization) {
  ::samson::add(record, "name", node_identifier_.str(), "different,left");
  ::samson::add(record, "user", user_, "different,left");
  ::samson::add(record, "connection", connection_type_, "different,left");

  if (socket_connection_->IsClosed()) {
    ::samson::add(record, "status", "disconnected", "different,left");
  } else {
    ::samson::add(record, "status", "connected", "different,left");
  } ::samson::add(record, "host", socket_connection_->host_and_port(), "different");

  ::samson::add(record, "In", au::str(rate_in_.hits(), "Ps") + " " + au::str(rate_in_.size(), "B"), "different");
  ::samson::add(record, "Out", au::str(rate_out_.hits(), "Ps") + " " + au::str(rate_out_.size(), "B"), "different");

  ::samson::add(record, "In (B/s)", (size_t)rate_in_.rate(), "f=uint64,sum");
  ::samson::add(record, "Out (B/s)", (size_t)rate_out_.rate(), "f=uint64,sum");

  // Pending data to be sent
  std::string queue_description = network_manager_->multi_packet_queue_.GetDescription(node_identifier_);
  ::samson::add(record, "Output queue", queue_description, "different");
}

std::string NetworkConnection::host() const {
  return socket_connection_->host();
}

int NetworkConnection::port() const {
  return socket_connection_->port();
}
}
