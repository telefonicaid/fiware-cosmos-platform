

#include "Channel.h"
#include "ServerConnection.h"  // Own interface
#include "StreamConnector.h"

namespace stream_connector {
ConnectionConnection::ConnectionConnection(Adaptor *item, ConnectionType type, std::string host, int port)
  : FileDescriptorConnection(item, type,  au::str("CONNECTION(%s:%d)", host.c_str(), port)) {
  host_ = host;
  port_ = port;
}

// FileDescriptorConnection virtual methods
au::FileDescriptor *ConnectionConnection::getFileDescriptor() {
  au::SocketConnection *socket_connection;
  au::Status s = au::SocketConnection::Create(host_
                                              , port_
                                              , &socket_connection);

  if (s == au::OK)
    return socket_connection; else
    return NULL;
}

// ---------------------------------
// ConnectionItem
// ---------------------------------

ConnectionItem::ConnectionItem(Channel *channel, ConnectionType type, std::string host, int port)
  : Adaptor(channel
            , type
            , au::str("CONNECTION(%s:%d)", host.c_str(), port)
            ) {
  host_ = host;
  port_ = port;
}

void ConnectionItem::start_item() {
  add(new ConnectionConnection(this, getType(), host_, port_));
}

void ConnectionItem::review_item() {
  // Nothing to do here
}

std::string ConnectionItem::getStatus() {
  return "OK";
}

void ConnectionItem::stop_item() {
  // Nothing to do here
}

// ---------------------------------
// StdinItem
// ---------------------------------


// Constructor & Destructor
StdinItem::StdinItem(Channel *_channel) : Adaptor(_channel, connection_input, "STDIN") {
}

// Information about status
std::string StdinItem::getStatus() {
  if (getNumConnections() > 0)
    return "connected"; return "closed";
}

void StdinItem::review_item() {
  // Nothing to do here
}

void StdinItem::start_item() {
  // Add connection
  add(new SimpleFileDescriptorConnection(this
                                         , connection_input
                                         , "stdin"
                                         , new au::FileDescriptor("stdin", 0)));

  // No more connections will be stablished, so consider it finished
  set_as_finished();
}

void StdinItem::stop_item() {
  // Nothing to do here
}

// ---------------------------------
// StdoutItem
// ---------------------------------

// Constructor & Destructor
StdoutItem::StdoutItem(Channel *_channel) : Adaptor(_channel, connection_output, "STDOUT") {
}

// Information about status
std::string StdoutItem::getStatus() {
  if (getNumConnections() > 0)
    return "connected"; return "closed";
}

void StdoutItem::start_item() {
  // Add connection
  add(new SimpleFileDescriptorConnection(this, connection_output, "stdout", new au::FileDescriptor("stdout", 1)));
}

void StdoutItem::review_item() {
  // Nothing to do here
}

void StdoutItem::stop_item() {
  // nothing to do here
}
}