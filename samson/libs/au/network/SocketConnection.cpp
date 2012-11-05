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
#include <arpa/inet.h>          // inet_ntoa
#include <errno.h>
#include <fcntl.h>              // fcntl, F_SETFD
#include <netdb.h>              // gethostbyname
#include <netinet/in.h>         // struct sockaddr_in
#include <netinet/tcp.h>        // TCP_NODELAY
#include <stdio.h>
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/types.h>          // types needed by socket include files
#include <sys/un.h>             // sockaddr_un
#include <unistd.h>             // close

#include "au/mutex/TokenTaker.h"

#include "SocketConnection.h"  // Own interface

namespace au {
SocketConnection::SocketConnection(int fd, std::string host, int port) :
  FileDescriptor((port == -1) ? host : au::str("%s:%d", host.c_str(),
                                               port),  fd) {
  host_ = host;
  port_ = port;
}

SocketConnection::~SocketConnection() {
  Close();   // Make sure is closed
}

std::string SocketConnection::str() {
  return au::str("Socket %s (fd=%d)", host_and_port().c_str(), fd());
}

std::string SocketConnection::host() {
  return host_;
}

int SocketConnection::port() {
  return port_;
}

std::string SocketConnection::host_and_port() {
  if (port_ != -1) {
    return au::str("%s:%d", host_.c_str(), port_);
  } else {
    return host_;
  }
}


SharedPointer<SocketConnection> SocketConnection::Create( const std::string& host , au::ErrorManager& error )
  {
    int fd;
    struct hostent *hp;
    struct sockaddr_in peer;

    std::string real_host;
    int real_port;
    
    size_t pos = host.find_last_of(":");
    if( pos == std::string::npos )
    {
      real_host = host;
      real_port = 80;
    }
    else
    {
      real_host = host.substr( 0 , pos );
      real_port = atoi( host.substr( pos+1 ).c_str() );
    };
    
    if( real_port == 0 )
    {
      error.set(au::str("No valid port (%d) in ´%s´", real_port ,  host.c_str() ));
      return SharedPointer<SocketConnection>(NULL);
    }
    
    if ((hp = gethostbyname(real_host.c_str())) == NULL) {
      error.set( au::str("Unknown host %s (gethostbyname returned error)" , host.c_str()) );
      return SharedPointer<SocketConnection>(NULL);
    }
    
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      error.set( au::str("Error creating socket, errno(%d): %s", errno, strerror(errno)));
      return SharedPointer<SocketConnection>(NULL);
    }
    
    memset((char *)&peer, 0, sizeof(peer));
    peer.sin_family      = AF_INET;
    peer.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    peer.sin_port        = htons(real_port);
    
    // Traces canceled since we are using this to connect to log server
    // LM_T(LmtSocketConnection, ("Connecting to worker at %s:%d", host.c_str(), port));
    
    // Try several times....
    int retries = 10;
    int tri     = 0;
    
    while (1) {
      if (::connect(fd, (struct sockaddr *)&peer, sizeof(peer)) == -1) {
        usleep(100000);
        if (tri > retries) {
          ::close(fd);
          // Traces canceled since we are using this to connect to log server
          error.set(au::str("Cannot connect to %s (even after %d retries)", host.c_str(), retries));
          return SharedPointer<SocketConnection>(NULL);
        }
        ++tri;
      } else {
        break;
      }
    }
    
    return SharedPointer<SocketConnection>( new SocketConnection(fd, real_host, real_port) );
  }
  
Status SocketConnection::Create(std::string host, int port, SocketConnection **socket_connection) {
  int fd;
  struct hostent *hp;
  struct sockaddr_in peer;

  if ((hp = gethostbyname(host.c_str())) == NULL) {
    *socket_connection = NULL;
    LM_E(("gethostbyname(%s): errno(%d): %s", host.c_str(), errno, strerror(errno)));
    return GetHostByNameError;
  }

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    *socket_connection = NULL;
    LM_E(("socket error, errno(%d): %s", errno, strerror(errno)));
    return SocketError;
  }

  memset((char *)&peer, 0, sizeof(peer));

  peer.sin_family      = AF_INET;
  peer.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
  peer.sin_port        = htons(port);

  // Traces canceled since we are using this to connect to log server
  // LM_T(LmtSocketConnection, ("Connecting to worker at %s:%d", host.c_str(), port));

  // Try several times....
  int retries = 10;
  int tri     = 0;

  while (1) {
    if (::connect(fd, (struct sockaddr *)&peer, sizeof(peer)) == -1) {
      usleep(100000);
      if (tri > retries) {
        ::close(fd);
        // Traces canceled since we are using this to connect to log server
        // LM_T(LmtSocketConnection,("Cannot connect to %s, port %d (even after %d retries)", host.c_str(), port, retries));
        *socket_connection = NULL;
        return ConnectError;
      }
      ++tri;
    } else {
      break;
    }
  }

  *socket_connection = new SocketConnection(fd, host, port);
  return OK;
}

size_t SocketConnection::GetConnectionTime() {
  return cronometer_.seconds();
}
}
