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


#include "LogProbe.h"  // Own interface

#include "au/au.pb.h"
#include "au/gpb.h"

namespace au {
void *RunLogProbe(void *p) {
  LogProbe *log_probe = (LogProbe *)p;

  log_probe->Run();
  return NULL;
}

void LogProbe::Clear() {
  Disconnect();
  // Finish all plugins
  au::map<std::string, LogProbePlugin>::iterator it;
  for (it = plugins_.begin(); it != plugins_.end(); ++it) {
    it->second->ProceassFinish();
  }
  plugins_.clearMap();
}

bool LogProbe::IsConnected() {
  if (socket_connection_ == NULL) {
    return false;
  }

  if (socket_connection_->IsClosed()) {
    return false;
  }

  return true;
}

void LogProbe::Disconnect() {
  if (socket_connection_ != NULL) {
    // Make sure background thread is gone
    socket_connection_->Close();
    void *ans;
    pthread_join(thread_id_, &ans);
    socket_connection_.Reset();
  }
}

void LogProbe::ConnectAsProbe(const std::string& host, const std::string& filter, au::ErrorManager& error) {
  if (socket_connection_ != NULL) {
    Disconnect();
  }

  // Add default port to host if necessary
  std::string str_host = host;
  if (str_host.find_last_of(":") == std::string::npos) {
    str_host += au::str(":%d", LOG_SERVER_DEFAULT_PORT);
  }

  // Create a socket connection with the provided host
  socket_connection_ = au::SocketConnection::Create(str_host, error);
  if (error.HasErrors()) {
    socket_connection_.Reset();
    return;
  }

  // Write a Hello message
  au::gpb::LogConnectionHello hello;
  hello.set_type(au::gpb::LogConnectionHello_LogConnectionType_LogProbe);
  hello.set_filter(filter);
  au::Status s = au::writeGPB(socket_connection_->fd(), &hello);
  if (s != au::OK) {
    error.AddError("Error writing hello message");
    socket_connection_->Close();
    return;
  }

  // Run thread in background
  au::ThreadManager::shared()->AddNonDetachedThread("log_proble", &thread_id_, NULL, RunLogProbe, this);
}

void LogProbe::ConnectAsQuery(const std::string& host, const std::string& filter, bool count,
                              au::ErrorManager& error) {
  if (socket_connection_ != NULL) {
    Disconnect();
  }

  // Add default port to host if necessary
  std::string str_host = host;
  if (str_host.find_last_of(":") == std::string::npos) {
    str_host += au::str(":%d", LOG_SERVER_DEFAULT_PORT);
  }

  // Create a socket connection with the provided host
  socket_connection_ = au::SocketConnection::Create(str_host, error);
  if (error.HasErrors()) {
    socket_connection_.Reset();
    return;
  }

  // Write a Hello message
  au::gpb::LogConnectionHello hello;
  hello.set_type(au::gpb::LogConnectionHello_LogConnectionType_LogQuery);
  hello.set_filter(filter);
  au::Status s = au::writeGPB(socket_connection_->fd(), &hello);
  if (s != au::OK) {
    error.AddError("Error writing hello message");
    socket_connection_->Close();
    return;
  }

  // Run thread in background
  au::ThreadManager::shared()->AddNonDetachedThread("log_proble", &thread_id_, NULL, RunLogProbe, this);
}

void LogProbe::Run() {
  // Infinite loop to receive logs
  while (true) {
    au::LogPointer log(new au::Log());
    bool real_log = log->Read(socket_connection_.shared_object());


    if (real_log) {
      Process(log);
    } else {
      error_.AddError("Error reading log");
      socket_connection_.Reset();
      return;
    }

    if (socket_connection_->IsClosed()) {
      socket_connection_.Reset();
      error_.AddError("Socket connection is closed");
      return;
    }
  }
}

void LogProbe::Process(au::SharedPointer<au::Log> log) {
  au::map<std::string, LogProbePlugin>::iterator it;
  for (it = plugins_.begin(); it != plugins_.end(); ++it) {
    it->second->Process(log);
  }
}

void LogProbe::AddPlugin(const std::string name, LogProbePlugin *plugin) {
  LogProbePlugin *old_plugin = plugins_.extractFromMap(name);

  if (old_plugin) {
    delete old_plugin;
  }
  plugins_.insertInMap(name, plugin);
}

void LogProbe::AddFilePlugin(const std::string& plugin_name, const std::string& file_name, au::ErrorManager &error) {
  int fd = open(file_name.c_str(), O_TRUNC | O_CREAT | O_WRONLY, 0644);

  if (fd > 0) {
    AddPlugin(plugin_name, new LogProbeFileDescriptor(fd));
  } else {
    error.AddError(au::str("Not possible to open file (%s)", strerror(errno)));
  }
}
}