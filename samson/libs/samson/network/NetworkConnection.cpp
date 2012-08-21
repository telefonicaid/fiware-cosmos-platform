
#include "au/ThreadManager.h"

#include "NetworkConnection.h"  // Own interface
#include "NetworkManager.h"
#include "samson/common/MessagesOperations.h"


namespace samson {
void *NetworkConnection_readerThread(void *p) {
  // Free resources automatically when this thread finish
  pthread_detach(pthread_self());

  NetworkConnection *network_connection = ( NetworkConnection * )p;
  network_connection->readerThread();
  network_connection->running_t_read = false;
  return NULL;
}

void *NetworkConnection_writerThread(void *p) {
  // Free resources automatically when this thread finish
  pthread_detach(pthread_self());

  NetworkConnection *network_connection = ( NetworkConnection * )p;
  network_connection->writerThread();
  network_connection->running_t_write = false;
  return NULL;
}

NetworkConnection::NetworkConnection(NodeIdentifier node_identifier
                                     , au::SocketConnection *socket_connection
                                     , NetworkManager *network_manager)
  : token_("NetworkConnection") {
  // Name in NetworkManager
  node_identifier_ = node_identifier;

  // Connection we are managing
  socket_connection_ = socket_connection;

  // Non identified
  node_identifier = NodeIdentifier(UnknownNode, 0);

  // Manager to report received messages
  network_manager_ = network_manager;

  // Init flags about threads
  running_t_read = false;
  running_t_write = false;

  // Create both threads for writing and reading
  if (!running_t_read) {
    std::string name = au::str("NetworkConnection::initReadWriteThreads::read (%s)"
                               , socket_connection->host_and_port().c_str());

    running_t_read = true;
    au::ThreadManager::shared()->addThread(name, &t_read, NULL, NetworkConnection_readerThread, this);
  }

  if (!running_t_write) {
    std::string name = au::str("NetworkConnection::initReadWriteThreads::write (%s)"
                               , socket_connection->host_and_port().c_str());

    running_t_write = true;
    au::ThreadManager::shared()->addThread(name, &t_write, NULL, NetworkConnection_writerThread, this);
  }
}

NetworkConnection::~NetworkConnection() {
  // Make sure everything in this class is gone
  CloseAndStopBackgroundThreads();

  if (running_t_read)
    LM_X(1, ("Deleting Network connection with a running read thread")); if (running_t_write)
    LM_X(1, ("Deleting Network connection with a running write thread")); delete socket_connection_;
}

// Wake up writer thread
void NetworkConnection::WakeUpWriter() {
  // Wake up writing thread if necessary
  au::TokenTaker tt(&token_);

  tt.WakeUpAll();
}

bool NetworkConnection::isDisconnectd() {
  return socket_connection_->IsClosed();
}

void NetworkConnection::Close() {
  // Make sure connection is close
  socket_connection_->Close();
}

void NetworkConnection::CloseAndStopBackgroundThreads() {
  // Make sure connection is close
  socket_connection_->Close();

  // Wake up writing thread if necessary
  {
    au::TokenTaker tt(&token_);
    tt.WakeUpAll();
  }

  // Wait until both thread are gone
  au::Cronometer cronometer;
  while (true) {
    if (!running_t_read || ( pthread_equal(pthread_self(), t_read)))
      if (!running_t_write || ( pthread_equal(pthread_self(), t_write)))
        return;

    usleep(100000);
    if (cronometer.seconds() > 1) {
      LM_W(("Waiting for background threads of connection %s", node_identifier_.getCodeName().c_str()));
      cronometer.Reset();
    }
  }
}

void NetworkConnection::readerThread() {
  while (1) {
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
    rate_in.push(total_read);

    if (s == au::OK) {
      packet->from = node_identifier_;   // Set from "node identifier"
      network_manager_->receive(this, packet);
    } else {
      socket_connection_->Close();   // Close connection since a packet has not been received correctly
    }
  }
}

void NetworkConnection::writerThread() {
  while (1) {
    // Quit if this connection is closed
    if (socket_connection_->IsClosed())
      return;

    // Get the next packet to be sent by me
    PacketPointer packet = network_manager_->multi_packet_queue.Front(node_identifier_);

    if (packet != NULL) {
      size_t total_write = 0;

      // Write the packet over this socket
      au::Status s = packet->write(socket_connection_, &total_write);

      // Monitor rate
      rate_out.push(total_write);

      if (s == au::OK)
        network_manager_->multi_packet_queue.Pop(node_identifier_);
    } else {
      // Block this thread until new packets are pushed or connection is restablish...
      au::TokenTaker tt(&token_);
      tt.Stop();   // block
    }
  }

  LM_X(1, ("Internal error"));  // No possible to get this line
}

std::string NetworkConnection::str() {
  std::ostringstream output;

  output << "[" << (running_t_read ? "R" : " ") << (running_t_write ? "W" : " ") << "]";

  if (socket_connection_->IsClosed())
    output << " Disconnected "; else
    output << " Connected    "; output << "[ " << node_identifier_.str() << " ] ";
  return output.str();
}

size_t NetworkConnection::get_rate_in() {
  return rate_in.getRate();
}

size_t NetworkConnection::get_rate_out() {
  return rate_out.getRate();
}

NodeIdentifier NetworkConnection::node_identifier() {
  return node_identifier_;
}

std::string NetworkConnection::host_and_port() {
  return socket_connection_->host_and_port();
}

void NetworkConnection::fill(gpb::CollectionRecord *record, const Visualization& visualization) {
  ::samson::add(record, "name", node_identifier_.getCodeName(), "different,left");
  ::samson::add(record, "user", user, "different,left");
  ::samson::add(record, "connection", connection_type, "different,left");

  if (socket_connection_->IsClosed())
    ::samson::add(record, "status", "disconnected", "different,left"); else
    ::samson::add(record, "status", "connected", "different,left"); ::samson::add(record, "host",
                                                                                  socket_connection_->host_and_port(),
                                                                                  "different");

  ::samson::add(record, "In (B)", rate_in.getTotalSize(), "f=uint64,sum");
  ::samson::add(record, "Out (B)", rate_out.getTotalSize(), "f=uint64,sum");

  ::samson::add(record, "In (B/s)", (size_t)rate_in.getRate(), "f=uint64,sum");
  ::samson::add(record, "Out (B/s)", (size_t)rate_out.getRate(), "f=uint64,sum");
}

std::string NetworkConnection::getName() {
  return node_identifier_.getCodeName();
}

std::string NetworkConnection::getHost() {
  return socket_connection_->host();
}

int NetworkConnection::getPort() {
  return socket_connection_->port();
}
}
