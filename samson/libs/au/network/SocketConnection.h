

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

#ifndef _H_AU_SOCKET_CONNECTION
#define _H_AU_SOCKET_CONNECTION

#include "au/statistics/Cronometer.h"
#include "au/containers/SharedPointer.h"
#include "au/Status.h"
#include "au/mutex/Token.h"
#include "au/network/FileDescriptor.h"
#include "au/string/StringUtilities.h"

/*****************************************************************************
*
*  SocketConnection
*
*  Handy class to work with a socket connection
*  It is used in NetworkListener and directly to establish an output connection
*  Note: Use only LM_LM or LM_LW in this class since it is used in hoock function
*
*****************************************************************************/

namespace au {
class SocketConnection : public FileDescriptor {
public:

  SocketConnection(int _fd, std::string _host, int _port);
  ~SocketConnection();

  // Main method to establish a new output socket connection
  static Status Create(std::string host, int port,
                       SocketConnection **socket_connection);

  // Secondary method to create a socket connection
  static SharedPointer<SocketConnection> Create( const std::string& host , au::ErrorManager& error );
  
  // Accesors
  std::string host();
  int port();
  std::string host_and_port();

  // Get connection time;
  size_t GetConnectionTime();

  // Debug information information
  std::string str();

private:

  au::Cronometer cronometer_;   // Creation time cronometer
  std::string host_;            // Name of this element for traces
  int port_;                    // Port in outputgoing connections ( -1 in receiving connections )
};
}

#endif  // ifndef _H_AU_SOCKET_CONNECTION
