

#include "au/Singleton.h"

#include "Adaptor.h"        // Own interface
#include "Channel.h"
#include "Connection.h"

namespace stream_connector {
Adaptor::Adaptor (Channel *_channel, ConnectionType _type, std::string description)
  : token("connector::Item") {
  channel = _channel;
  type = _type;
  description_ = description;

  next_id = 0;

  canceled = false;       // Default value
  finished = false;
}

Adaptor::~Adaptor() {
  // Cancel this item to make sure we do not remove with any thread running here
  cancel_item();

  // Remover all connections
  connections.clearMap();
}

ConnectionType Adaptor::getType() {
  return type;
}

std::string Adaptor::getName() {
  return name_;
}

std::string Adaptor::getFullName() {
  return au::str("%s.%s",  channel->getName().c_str(), name_.c_str());
}

std::string Adaptor::getDescription() {
  return description_;
}

const char *Adaptor::getTypeStr() {
  return str_ConnectionType(type);
}

void Adaptor::add(Connection *connection) {
  au::TokenTaker tt(&token);

  connections.insertInMap(next_id, connection);
  connection->id = next_id;

  log("Message", au::str("Connection %s (%s) added"
                         , connection->getFullName().c_str()
                         , connection->getDescription().c_str()));

  // Init the connection properly
  connection->init_connecton();

  next_id++;
}

void Adaptor::push(engine::BufferPointer buffer) {
  au::TokenTaker tt(&token);

  au::map<int, Connection>::iterator it_connections;
  for (it_connections = connections.begin()
       ; it_connections != connections.end()
       ; it_connections++)
  {
    Connection *connection = it_connections->second;
    if (connection->getType() == connection_output) {
      connection->push(buffer);
    }
  }
}

void Adaptor::review() {
  if (canceled) {
    return;         // Not call review
  }
  if (!finished) {
    review_item();  // Review all connections
  }
  {
    au::TokenTaker tt(&token);
    au::map<int, Connection>::iterator it_connections;

    for (it_connections = connections.begin()
         ; it_connections != connections.end()
         ; it_connections++)
    {
      Connection *connection = it_connections->second;
      connection->review();
    }
  }
}

void Adaptor::init_item() {
  start_item();
};
void Adaptor::cancel_item() {
  canceled = true;       // This will block future calls to review

  stop_item();
  {
    // Mutex protectio
    au::TokenTaker tt(&token);

    // Cancel all connections
    au::map<int, Connection>::iterator it_connections;
    for (it_connections = connections.begin()
         ; it_connections != connections.end()
         ; it_connections++)
    {
      Connection *connection = it_connections->second;
      connection->cancel_connecton();
    }
  }
}

int Adaptor::getNumConnections() {
  au::TokenTaker tt(&token);

  return connections.size();
}

void Adaptor::set_as_finished() {
  if (finished) {
    return;
  }

  // Log activity
  log("Message", "Set as finished");
  finished = true;
}

bool Adaptor::is_finished() {
  return finished;
}

size_t Adaptor::getConnectionsBufferedSize() {
  au::TokenTaker tt(&token);
  size_t total = 0;

  au::map<int, Connection>::iterator it_connections;
  for (it_connections = connections.begin()
       ; it_connections != connections.end()
       ; it_connections++)
  {
    Connection *connection = it_connections->second;
    total += connection->bufferedSize();
  }

  return total;
}

void Adaptor::remove_finished_connections(au::ErrorManager *error) {
  // Mutex protectio
  au::TokenTaker tt(&token);

  // Cancel all connections
  au::map<int, Connection>::iterator it_connections;
  for (it_connections = connections.begin()
       ; it_connections != connections.end()
       ; it_connections++)
  {
    Connection *connection = it_connections->second;
    if (connection->is_finished()) {
      log("Message", au::str("Removing connection %s", connection->getFullName().c_str()));

      connection->cancel_connecton();
      delete connection;
      connections.erase(it_connections);
    }
  }
}

// Log system
void Adaptor::log(std::string type, std::string message) {
  log(au::SharedPointer<Log> (new Log(getFullName(), type, message)));
}

void Adaptor::log(au::SharedPointer<Log> log) {
  LogManager *log_manager = au::Singleton<LogManager>::shared();

  log_manager->log(log);
}

void Adaptor::report_output_size(size_t size) {
  traffic_statistics.push_output(size);
  channel->report_output_size(size);
}

void Adaptor::report_input_size(size_t size) {
  traffic_statistics.push_input(size);
  channel->report_input_size(size);
}

bool Adaptor::accept(InputInterChannelConnection *connection) {
  // By default, we do not accept interchannel connections
  // Only specific items accept this type of connection
  return false;
}

Connection *Adaptor::getFirstConnection() {
  if (connections.size() == 0) {
    return NULL;
  }
  return connections.begin()->second;
}
}