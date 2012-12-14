
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

/*
 * FILE            ThreadManager
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  Classes to controll current threads working for this app.
 */

#include <fcntl.h>              // F_SETFD
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>             // close

#include "NetworkListener.h"  // Own interface
#include "au/Log.h"
#include "au/ThreadManager.h"
#include "au/log/LogMain.h"
#include "au/network/SocketConnection.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "samson/common/Logs.h"

namespace au {
void *NetworkListener_run(void *p);

NetworkListener::NetworkListener(NetworkListenerInterface *network_listener_interface)
  : au::Thread("NetworkListener") {
  network_listener_interface_ = network_listener_interface;
  rFd_ = -1;   // Init value
  port_ = -1;
}

NetworkListener::~NetworkListener() {
  // make sure the background thread is not working
  StopNetworkListener();
}

void NetworkListener::StopNetworkListener() {
  // Close file descriptor
  if (rFd_ != -1) {
    int rc = ::close(rFd_);
    if (rc) {
      LOG_W(logs.listener, ("Error closing fd %d in network listener over port %d ( rc %d )", rFd_, port_, rc));
    }
    rFd_ = -1;
  }
  port_ = -1;
  StopThread();  // Stop background thread if still running
}

Status NetworkListener::InitNetworkListener(int port) {
  if (port_ != -1) {
    LOG_W(logs.listener, ("NetworkListener previously initialized with port %d. Ignoring...", port_));
    return au::Error;
  }


  int reuse = 1;
  struct sockaddr_in sock;
  struct sockaddr_in peer;

  if (rFd_ != -1) {
    LOG_W(logs.listener, ("This listener already seems to be prepared, so not init again"));
    return Error;
  }

  if ((rFd_ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    LM_RP(SocketError, ("socket"));
  }

  fcntl(rFd_, F_SETFD, 1);

  memset(reinterpret_cast<char *>(&sock), 0, sizeof(sock));
  memset(reinterpret_cast<char *>(&peer), 0, sizeof(peer));

  sock.sin_family      = AF_INET;
  sock.sin_addr.s_addr = INADDR_ANY;
  sock.sin_port        = htons(port);

  setsockopt(rFd_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&reuse), sizeof(reuse));

  if (bind(rFd_, reinterpret_cast<struct sockaddr *>(&sock), sizeof(struct sockaddr_in)) == -1) {
    ::close(rFd_);
    rFd_ = -1;
    // LM_RP() makes the return of Status BindError and it also prints strerror(errno)
    LOG_V(logs.listener, ("Error in bind to port %d", port));
    return BindError;
  }

  if (listen(rFd_, 10) == -1) {
    ::close(rFd_);
    rFd_ = -1;
    // LM_RP() makes the return of Status ListenError and it also prints strerror(errno)
    LM_RP(ListenError, ("Error listening on port %d", port));
  }

  port_ = port;   // Keep port information
  StartThread();  // Star background thread to accept connections

  return au::OK;
}

void NetworkListener::RunThread() {
  fd_set rFds;
  int max;
  struct timeval tv;

  int fds;

  while (true) {
    // this means that stop has been called
    int rFd = rFd_;
    if (rFd == -1) {
      return;
    }

    // One fd to read connections
    FD_ZERO(&rFds);
    max = rFd;
    FD_SET(rFd, &rFds);

    // Timeout
    tv.tv_sec  = 1;
    tv.tv_usec = 0;

    // Main select to wait new connections
    fds = select(max + 1, &rFds, NULL, NULL, &tv);

    if ((fds == -1) && (errno == EINTR)) {
      continue;
    }
    if (fds == -1) {
      continue;  // We wil continue until rFd_ == -1
      /*
       * switch (errno) {
       * case EAGAIN:return (void*)"EAGAIN";
       * case EBADF:return (void*)"EBADF";
       * case EINTR:return (void*)"EINTR";
       * case EINVAL:return (void*)"EINVAL";
       * default:
       *  return (void*)"select == -1";   // Finish thread since there is a problem with fd
       *  break;
       * }
       */
    }
    if (fds == 0) {
      // timeout();
      continue;
    }
    if (!FD_ISSET(rFd, &rFds)) {
      LM_X(1, ("Error in main loop to accept connections"));  // Accept a new connection
    }
    SocketConnection *socket_connection = AcceptNewNetworkConnection();

    // Notify this new connection
    if (socket_connection) {
      network_listener_interface_->newSocketConnection(this, socket_connection);
    }
  }
}

void ip2string(int ip, char *ipString, int ipStringLen) {
  snprintf(ipString, ipStringLen, "%d.%d.%d.%d",
           ip & 0xFF,
           (ip & 0xFF00) >> 8,
           (ip & 0xFF0000) >> 16,
           (ip & 0xFF000000) >> 24);
}

SocketConnection *NetworkListener::AcceptNewNetworkConnection(void) {
  int fd;
  struct sockaddr_in sin;
  char hostName[64];
  unsigned int len         = sizeof(sin);
  int hostNameLen = sizeof(hostName);

  memset(reinterpret_cast<char *>(&sin), 0, len);

  int rFd = rFd_;

  if (rFd == -1) {
    return NULL;
  }

  if ((fd = ::accept(rFd, (struct sockaddr *)&sin, &len)) == -1) {
    LM_RP(NULL, ("accept"));
  }
  ip2string(sin.sin_addr.s_addr, hostName,
            hostNameLen);

  // Create the connection
  SocketConnection *socket_connection = new SocketConnection(fd, hostName, -1);
  return socket_connection;
}

// Check running status
bool NetworkListener::IsNetworkListenerRunning() const {
  return IsThreadRunning();
}

int NetworkListener::port() const {
  return port_;
}
}
