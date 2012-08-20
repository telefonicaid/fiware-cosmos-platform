
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

#ifndef _H_AU_NETWORK_SERVICE
#define _H_AU_NETWORK_SERVICE

#include "au/Status.h"
#include "au/containers/set.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "au/tables/Table.h"

#include "au/Environment.h"
#include "au/au.pb.h"
#include "au/gpb.h"

#include "au/string.h"
#include "au/utils.h"

#include "au/console/Console.h"

#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"


/*****************************************************************************
*
*  Service
*
*  Multiple connection support over an open port
*  -> Multiple connections are accepted over a port
*  -> Connections are managed automatically by this class
*  -> A simple method is called to implement the service
*
*  virtual void run( SocketConnection * soket_connection , bool *quit )=0;
*
*****************************************************************************/

namespace au {
namespace network {
class Service;

class ServiceItem {
public:

  ServiceItem(Service *_service,
              SocketConnection *_socket_connection);
  virtual ~ServiceItem();

  // Stop this connection
  void RunInBackground();
  void Stop();

private:

  pthread_t t_;              // Background thread
  bool quit_;                // Flag to indicate that this item should exit background thread

  Service *service_;
  SocketConnection *socket_connection_;

  friend void *run_service_item(void *p);
  friend class Service;
};

// Service opening a paricular port and accepting connections

class Service : public NetworkListenerInterface {
public:

  // Constructor with the port to open
  Service(int _port);
  ~Service();

  Status InitService();              // Init the service
  void StopService();                // Stop all threads for connections and thread for listener

  // Main method to define the service itself
  virtual void run(SocketConnection *soket_connection,
                   bool *quit) = 0;

  // virtual methods of NetworkListenerInterface
  virtual void newSocketConnection(
    NetworkListener *_listener,
    SocketConnection *socket_connetion);

  // Accessorts
  int port();

  // Debug information
  std::string str();
  std::string GetStringStatus();
  au::tables::Table *getConnectionsTable();

private:

  // Method called by thread running item in background
  void finish(ServiceItem *item);

  au::Token token_;                 // Mutex protection ( list of items )

  int port_;                        // Port to offer this service
  bool init_;                       // Flag to avoid multiple inits
  NetworkListener listener_;        // Listener to receive connections
  au::set< ServiceItem > items_;    // Connected items

  friend class ServiceItem;
  friend void *run_service_item(void *p);
};
}
}

#endif  // ifndef _H_AU_NETWORK_SERVICE
