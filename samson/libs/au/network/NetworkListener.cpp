
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

#include <fcntl.h>              // F_SETFD
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>             // close

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "NetworkListener.h"  // Own interface
#include "au/ThreadManager.h"
#include "au/network/SocketConnection.h"

namespace au {
void *NetworkListener_run(void *p);

NetworkListener::NetworkListener(
  NetworkListenerInterface *network_listener_interface) {
  network_listener_interface_ = network_listener_interface;

  rFd_ = -1;   // Init value
  port_ = -1;

  // Thread to indicate if we are really listening connections
  background_thread_running_ = false;
}

NetworkListener::~NetworkListener() {
  // make sure the background thread is not working
  StopNetworkListener();
}

void NetworkListener::StopNetworkListener() {
  if (!background_thread_running_) {
    return;   // Nothing to do
  }
  // Close the open file descriptor
  int rc = ::close(rFd_);
  if( rc )
    LM_W(("Error closing fd %d in network listener over port %d ( rc %d )" , rFd_ , port_ , rc ));
  rFd_ = -1;

  // Joint the background thread
  void *r;
  LM_LW(( "Joining background thread of listener on port %d to finish\n", port_ ));
  pthread_join(t, &r);
}

Status NetworkListener::InitNetworkListener(int port) {
  // If we are running something in background, we do not accept bind to another port
  if (background_thread_running_) {
    if (port_ != port)
      LM_W((
             "NetworkListener already listening on port %s. Ignoring request to open port %d",
             port_, port )); return au::Error;
  }

  // Keep port information
  port_ = port;

  int reuse = 1;
  struct sockaddr_in sock;
  struct sockaddr_in peer;

  if (rFd_ != -1) {
    LM_W(("This listener already seems to be prepared, so not init again"));
    return Error;
  }

  if ((rFd_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    LM_RP(SocketError, ("socket")); fcntl(rFd_, F_SETFD, 1);

  memset(&sock, 0, sizeof(sock));
  memset(&peer, 0, sizeof(peer));

  sock.sin_family      = AF_INET;
  sock.sin_addr.s_addr = INADDR_ANY;
  sock.sin_port        = htons(port);

  setsockopt(rFd_, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));

  if (bind(rFd_, (struct sockaddr *)&sock, sizeof(struct sockaddr_in)) == -1) {
    ::close(rFd_);
    rFd_ = -1;
    // LM_RP(BindError, ("bind to port %d: %s", port, strerror(errno)));
    return BindError;
  }

  if (listen(rFd_, 10) == -1) {
    ::close(rFd_);
    rFd_ = -1;
    // LM_RP(ListenError, ("listen to port %d", port));
    return ListenError;
  }


  // Create thread
  LM_T(LmtCleanup, ("Creating a thread"));
  au::ThreadManager::shared()->addNonDetachedThread(au::str(
                                                      "NetworkListener on port %d",
                                                      port)
                                                    , &t
                                                    , NULL
                                                    , NetworkListener_run
                                                    , this);

  background_thread_running_ = true;

  return OK;
}

void *NetworkListener_run(void *p) {
  NetworkListener *network_listener = (NetworkListener *)p;

  network_listener->runNetworkListener();
  network_listener->background_thread_running_ = false;
  return NULL;
}

void NetworkListener::runNetworkListener() {
  if (rFd_ == -1) {
    return;   // It was closed before running
  }
  fd_set rFds;
  int max;
  struct timeval tv;

  int fds;
  int eps;

  while (true) {
    // this means that stop has been called
    int rFd = rFd_;
    if (rFd == -1)
      return;

    // One fd to read connections
    FD_ZERO(&rFds);
    eps = 1;
    max = rFd;
    FD_SET(rFd, &rFds);

    // Timeout
    tv.tv_sec  = 1;
    tv.tv_usec = 0;

    // Main select to wait new connections
    fds = select(max + 1,  &rFds, NULL, NULL, &tv);

    if ((fds == -1) && (errno == EINTR))
      continue; if (fds == -1) {
      return;   // Finish thread since this is an error
    }
    if (fds == 0)
      // timeout();
      continue; if (!FD_ISSET(rFd, &rFds)) {
      LM_X(1, ("Error in main loop to accept connections"));  // Accept a new connection
    }
    SocketConnection *socket_connection = acceptNewNetworkConnection();

    // Notify this new connection
    if (socket_connection)
      network_listener_interface_->newSocketConnection(this, socket_connection);
  }
}

void ip2string(int ip, char *ipString, int ipStringLen) {
  snprintf(ipString, ipStringLen, "%d.%d.%d.%d",
           ip & 0xFF,
           (ip & 0xFF00) >> 8,
           (ip & 0xFF0000) >> 16,
           (ip & 0xFF000000) >> 24);
}

SocketConnection *NetworkListener::acceptNewNetworkConnection(void) {
  int fd;
  struct sockaddr_in sin;
  char hostName[64];
  unsigned int len         = sizeof(sin);
  int hostNameLen = sizeof(hostName);

  memset((char *)&sin, 0, len);

  int rFd = rFd_;

  if (rFd == -1)
    return NULL; LM_T(LmtNetworkListener, ("Accepting incoming connection"));
  if ((fd = ::accept(rFd, (struct sockaddr *)&sin, &len)) == -1)
    LM_RP(NULL, ("accept")); ip2string(sin.sin_addr.s_addr, hostName,
                                       hostNameLen);

  // Create the connection
  SocketConnection *socket_connection = new SocketConnection(fd, hostName, -1);
  return socket_connection;
}

// Check running status
bool NetworkListener::IsNetworkListenerRunning() const {
  return background_thread_running_;
}

int NetworkListener::port() const {
  return port_;
}
}
