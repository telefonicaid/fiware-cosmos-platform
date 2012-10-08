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
