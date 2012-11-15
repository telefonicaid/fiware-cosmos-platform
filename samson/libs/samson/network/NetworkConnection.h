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


#ifndef _H_SAMSON_NETWORK_CONNECTION
#define _H_SAMSON_NETWORK_CONNECTION

#include <string>

#include "au/statistics/Rate.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "au/network/SocketConnection.h"

#include "samson/common/Visualitzation.h"
#include "samson/common/status.h"
#include "samson/network/Packet.h"
#include "samson/network/PacketQueue.h"

namespace samson {
class NetworkManager;

class NetworkConnection {
public:

  // Constructor & Destructor
  NetworkConnection(NodeIdentifier node_identifier
                    , au::SocketConnection *socket_connection
                    , NetworkManager *network_manager);

  ~NetworkConnection();

  // Close connection ( if still open ) and wait until threads are gone
  void CloseAndStopBackgroundThreads();

  // Close socket ( no waiting for backgroud threads )
  void Close();

  // Wake up the writer thrad
  void WakeUpWriter();

  // Check if the socket is closed
  bool isDisconnectd();

  // Debug string
  std::string str();

  // Get some information
  std::string name() const;
  std::string host() const;
  int port() const;
  size_t rate_in() const;
  size_t rate_out() const;
  NodeIdentifier node_identifier()  const;
  std::string host_and_port()  const;

  // Function to generate lists of items in delilah console
  void fill(gpb::CollectionRecord *record, const Visualization& visualization);

private:

  // Friend functions to run both read and write threads
  friend void *NetworkConnection_writerThread(void *p);
  friend void *NetworkConnection_readerThread(void *p);

  // Read & Write threads main functions
  void readerThread();
  void writerThread();

  // Identifier of the node ( if available )
  NodeIdentifier node_identifier_;

  friend class NetworkManager;
  friend class CommonNetwork;

  // Socket Connection
  au::SocketConnection *socket_connection_;
  
  // User and password for this connection
  std::string user_;
  std::string password_;
  std::string connection_type_;

  // Token to block write thread when more packets have to be sent
  au::Token token_;

  // Pointer to the manager to report received messages
  // and extrace message to send
  NetworkManager *network_manager_;

  // Threads for reading and writing packets to this socket
  pthread_t t_read_, t_write_;
  bool running_t_read_;        // Flag to indicate that there is a thread using this endpoint writing data
  bool running_t_write_;       // Flag to indicate that there is a thread using this endpoint reading data

  // Information about rate
  au::rate::Rate rate_in_;
  au::rate::Rate rate_out_;
};
}

#endif  // ifndef _H_SAMSON_NETWORK_CONNECTION
