

#ifndef _H_SAMSONPUSH_CONNECTIONS_MANAGER
#define _H_SAMSONPUSH_CONNECTIONS_MANAGER

#include "au/mutex/TokenTaker.h"

#include "au/network/NetworkListener.h"
#include "samson/client/SamsonClient.h"
#include "samson/client/SamsonPushBuffer.h"

// Function to run the background thread
void *run_SamsonPushConnection(void *p);

class SamsonPushConnection {
  samson::SamsonPushBuffer *pushBuffer;
  au::SocketConnection *socket_connetion;
  char *buffer;
  bool thread_running;

public:

  SamsonPushConnection(au::SocketConnection *_socket_connetion);
  ~SamsonPushConnection();

  void run();

  bool isFinished() {
    return !thread_running;
  }
};

class SamsonPushConnectionsManager : public au::NetworkListenerInterface {
  au::Token token;
  std::set<SamsonPushConnection *> connections;

public:

  SamsonPushConnectionsManager() : token("SamsonPushConnectionsManager") {
  }

  virtual void newSocketConnection(au::NetworkListener *listener, au::SocketConnection *socket_connetion) {
    listener = NULL;

    // Mutex protection
    au::TokenTaker tt(&token);

    SamsonPushConnection *connection = new SamsonPushConnection(socket_connetion);
    connections.insert(connection);
  }

  void review_connections() {
    // Mutex protection
    au::TokenTaker tt(&token);

    std::set<SamsonPushConnection *>::iterator it_connections;
    for (it_connections = connections.begin(); it_connections != connections.end(); it_connections++) {
      SamsonPushConnection *connection = *it_connections;
      if (connection->isFinished()) {
        connections.erase(it_connections);
      }
    }
  }

  size_t getNumConnections() {
    // Mutex protection
    au::TokenTaker tt(&token);

    return connections.size();
  }
};

#endif // ifndef _H_SAMSONPUSH_CONNECTIONS_MANAGER
