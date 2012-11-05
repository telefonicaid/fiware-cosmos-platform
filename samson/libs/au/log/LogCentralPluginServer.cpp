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

#include "au/log/LogCentralPluginServer.h"  // Own interface

#include "au/au.pb.h"
#include "au/gpb.h"

namespace au {
LogCentralPluginServer::LogCentralPluginServer(const std::string& host, int port,
                                 const std::string& local_file) : LogCentralPlugin("Server") {
  set_host(host, port, local_file);
}

void LogCentralPluginServer::set_host(const std::string& host, int port, const std::string& local_file) {
  host_ = host;
  port_ = port;
  local_file_ = local_file;

  // Close previous connections if any
  local_file_descriptor_ = NULL;
  socket_connection_ = NULL;

  // Open local file ( if possible )
  int fd = open(local_file_.c_str(), O_WRONLY | O_CREAT, 0644);
  if (fd >= 0) {
    local_file_descriptor_ = new FileDescriptor("local_log", fd);
  } else {
    local_file_descriptor_ = NULL;
    // LM_LW(("Not possible to open local log file %s. Logs will be definitely lost", local_file_.c_str()));
  }

  // Connect the next time a trace is sent
  time_reconnect_ = 0;
}

LogCentralPluginServer::~LogCentralPluginServer() {
  // Not necessary to remove ( shared pointers used )
}

void SetLogServer(std::string _host, int _port);

std::string LogCentralPluginServer::host() const {
  return host_;
}

int LogCentralPluginServer::port() const {
  return port_;
}

std::string LogCentralPluginServer::local_file() const {
  return local_file_;
}

void LogCentralPluginServer::SetLogServer(std::string host, int port) {
  host_ = host;
  port_ = port;
  socket_connection_ = NULL;
  time_reconnect_ = 0;
}

void LogCentralPluginServer::ReviewSocketConnection() {
  // If already connected, nothing to do
  if ((socket_connection_ != NULL) && !socket_connection_->IsClosed()) {
    return;
  }

  // If connected but closed, remove it
  if ((socket_connection_ != NULL) && socket_connection_->IsClosed()) {
    socket_connection_ = NULL;   // Socket is closed automatically on destructor

    time_since_last_connection_.Reset();      // This is the time counter since last connection
    time_reconnect_ = 0;                      // Force try reconnect in next log
  }

  if (socket_connection_ == NULL) {
    // Reconnect to server if necessary
    size_t time = time_since_last_connection_.seconds();
    if (( host_ != "" )  && ( time >= time_reconnect_ )) {
      SocketConnection *tmp_socket_connection;
      au::Status s = au::SocketConnection::Create(host_, port_, &tmp_socket_connection);

      if (s != au::OK) {
        // Compute new time to try connection
        if (time == 0) {
          time_reconnect_ = 120;   // try again in 2 minutes
        }
        if (time_reconnect_ < time) {
          time_reconnect_ = time;

          // Increase time to reconnect
          time_reconnect_ *= 2;

          /*
           * size_t next_try_time = time_reconnect_ - time;
           * LM_W(("Not possible to connect with log server %s:%d (%s). Next try in %s"
           * , host_.c_str(), port_, au::status(s), au::str_time(next_try_time).c_str()));
           */
        }
      } else {
        // Write hello message
        gpb::LogConnectionHello hello;
        hello.set_type(gpb::LogConnectionHello_LogConnectionType_LogProvider);
        au::Status s = writeGPB(tmp_socket_connection->fd(), &hello);
        if (s == au::OK) {
          socket_connection_.Reset(tmp_socket_connection);
        }
      }
    }
  }
}

void LogCentralPluginServer::Emit(SharedPointer<Log> log) {
  // review connection with server if necessary
  ReviewSocketConnection();

  // Try socket first...
  if (socket_connection_ != NULL) {
    if (log->Write(socket_connection_.shared_object())) {
      return;
    }
    else
      socket_connection_ = NULL;
  }

  // It was not possible, so use disk
  if (local_file_descriptor_ != NULL) {
    if (!log->Write(local_file_descriptor_.shared_object())) {
      // Local log failed...
    }
  }
}

std::string LogCentralPluginServer::status() {
  bool connected = (socket_connection_ != NULL);

  return au::str("Host: %s:%d (%s)"
                 , host_.c_str()
                 , port_
                 , connected ? "OK" : "[non connnected]"
                 );
}
}
