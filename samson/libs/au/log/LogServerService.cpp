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
#include "au/log/LogServerService.h"  // Own interface

#include <time.h>

#include "logMsg/logMsg.h"                 // LM_T
#include "logMsg/traceLevels.h"            // LmtFileDescriptors, etc.

#include "au/au.pb.h"
#include "au/containers/vector.h"
#include "au/log/LogQuery.h"

#define Char_to_int(x) ((x) - 48)

namespace au {
LogServerService::LogServerService(int port, const std::string& _directory) :
  network::Service(port), token("LogServerService"),
  token_log_probe_connections_("LogServerSerice::token_log_probe_connections_") {
  directory = _directory;

  file_counter = 0;
  current_size = 0;
  fd = NULL;
}

LogServerService::~LogServerService() {
}

std::string LogServerService::getFileNameForLogFile(int counter) {
  return au::str("%s/log_%d", directory.c_str(), counter);
}

void LogServerService::openFileDescriptor(au::ErrorManager *error) {
  // Check if disconnected
  if (fd) {
    if (fd->IsClosed()) {
      delete fd;
      fd = NULL;
    }
  }
  if (!fd) {
    std::string current_file_name;

    while (true) {
      // Get the name of the log file
      current_file_name = getFileNameForLogFile(file_counter++);

      struct stat buf;
      int s = stat(current_file_name.c_str(), &buf);
      if (s == 0) {
        continue;             // File exist, so let's try the next number...
      }
      int _fd = open(current_file_name.c_str(), O_CREAT | O_WRONLY, 0644);

      if (_fd < 0) {
        error->AddError(au::str("Error opening file %s (%s)", current_file_name.c_str(), strerror(errno)));
        return;
      }

      // Create a file descriptor
      fd = new au::FileDescriptor("log fd", _fd);

      // Init byte counter for the current file
      current_size = 0;

      // File created!
      return;
    }
  }
}

void LogServerService::initLogServerService(au::ErrorManager *error) {
  // Create directory
  if ((mkdir(directory.c_str(), 0755) != 0) && (errno != EEXIST)) {
    error->AddError(au::str("Error creating directory %s (%s)", directory.c_str(), strerror(errno)));
    return;
  }

  // Open the first file-descriptor
  openFileDescriptor(error);

  if (error->HasErrors()) {
    return;
  }

  au::Status s = InitService();
  if (s != au::OK) {
    error->AddError(au::str("Error initializing server (%s)", au::status(s)));
    return;
  }
}

void LogServerService::Process(LogQuery *log_query) {
  log_container_.Process(log_query);
}

void LogServerService::runLogProbe(gpb::LogConnectionHello *hello, au::SocketConnection *socket_connection,
                                   bool *quit) {
  // Prove connection
  std::string filter;

  if (hello->has_filter()) {
    filter = hello->filter();
  }

  LogProbeConnection log_prove_connection(filter);

  {
    au::TokenTaker tt(&token_log_probe_connections_);
    log_probe_connections_.insert(&log_prove_connection);
  }

  while (!*quit) {
    LogPointer log = log_prove_connection.Pop();

    if (log != NULL) {
      if (!log->Write(socket_connection)) {
        break;  // Not possible to write to this channel
      }
    } else {
      usleep(100000);
    }
    if (socket_connection->IsClosed()) {
      break;
    }
  }

  // Remove connection from log_connections vector
  {
    au::TokenTaker tt(&token_log_probe_connections_);
    log_probe_connections_.erase(&log_prove_connection);
  }
}

void LogServerService::runLogQuery(gpb::LogConnectionHello *hello, au::SocketConnection *socket_connection,
                                   bool *quit) {
  std::string query_definition = hello->filter();   // Definition of the query

  LogQuery log_query(hello->filter());

  // Process all required logs
  Process(&log_query);

  // Emit logs to the socket connection
  log_query.Emit(socket_connection);
}

void LogServerService::runLogProvider(gpb::LogConnectionHello *hello, au::SocketConnection *socket_connection,
                                      bool *quit) {
  // Provider
  while (!*quit) {
    // Read a log
    au::SharedPointer<Log>log(new Log());
    if (!log->Read(socket_connection)) {
      // LM_V(("Closed connection from %s", socket_connection->host_and_port().c_str()));
      return;    // Not possible to read a log...
    }

    std::string channel = au::str("%s_%s_%d"
                                  , log->Get("EXEC").c_str()
                                  , socket_connection->host_and_port().c_str()

                                  , log->log_data().pid);
    // Additional information to the log
    log->Set("channel", channel);
    log->Set("host", socket_connection->host_and_port());    // Additional information to the log

    // Push logs to probes
    {
      au::TokenTaker tt(&token_log_probe_connections_);
      std::set<LogProbeConnection *>::iterator iterator;
      for (iterator = log_probe_connections_.begin(); iterator != log_probe_connections_.end(); iterator++) {
        (*iterator)->Push(log);
      }
    }

    // Add log...
    add(log);
  }
}

void LogServerService::run(au::SocketConnection *socket_connection, bool *quit) {
  // Read initial "hello" packet to understand what type of connection we have
  // - log provider
  // - log probe
  // - log query

  gpb::LogConnectionHello *hello;
  au::Status s = readGPB(socket_connection->fd(), &hello, 10);  // 10 seconds timeout to read hello message

  if (s != au::OK) {
    LOG_SW(("Error reading hello message for incoming connection (%s)", au::status(s)));
    return;
  }

  if (hello->type() == au::gpb::LogConnectionHello_LogConnectionType_LogProbe) {
    runLogProbe(hello, socket_connection, quit);
    return;
  }
  if (hello->type() == au::gpb::LogConnectionHello_LogConnectionType_LogQuery) {
    runLogQuery(hello, socket_connection, quit);
    return;
  }
  if (hello->type() == au::gpb::LogConnectionHello_LogConnectionType_LogProvider) {
    runLogProvider(hello, socket_connection, quit);
    return;
  }
}

void LogServerService::add(au::SharedPointer<Log> log) {
  // Mutex protection
  au::TokenTaker tt(&token);

  // Push log to the on-memory log
  log_container_.Push(log);

  // Monitorize rate of logs
  rate.Push(log->SerialitzationSize());

  // Check max size for file
  if (fd) {
    if (current_size > 64000000) {
      fd->Close();
      delete fd;
      fd = NULL;
    }  // Open if necessary
  }
  if (!fd) {
    current_size = 0;

    au::ErrorManager error;
    openFileDescriptor(&error);
    if (error.HasErrors()) {
      LOG_SW(("Not possible to open local file to save logs. Logs will be deninitelly lost"));
      return;
    }
  }

  // Write to file
  if (fd) {
    log->Write(fd);
  }
  current_size += log->SerialitzationSize();
}

void LogServerService::addNewSession() {
  au::SharedPointer<Log> log(new Log());
  log->SetNewSession();
  add(log);

  // Clear on memory logs
  log_container_.Clear();
}

class ChannelInfo {
public:

  std::string name_;

  int logs_;            // Number of logs
  size_t size_;         // Total size

  au::Descriptors descriptors_;

  std::string time_;       // Most recent time stamp

  ChannelInfo(std::string name, au::SharedPointer<Log> log) {
    name_ = name;

    logs_ = 0;
    size_ = 0;

    // First time
    time_ = log->Get("date") + " " + log->Get("time");

    add_log(log);
  }

  void add_log(au::SharedPointer<Log> log) {
    logs_++;
    size_ += log->SerialitzationSize();

    std::string type = log->Get("type");
    if (type == "") {
      type = "?";
    }
    descriptors_.Add(type);
  }
};

std::string LogServerService::GetInfoTable() {
  au::TokenTaker tt(&token);

  return log_container_.log_counter().GetInfoTable();
}

std::string LogServerService::GetChannelsTable() {
  au::TokenTaker tt(&token);

  return log_container_.log_counter().GetChannelsTable();
}
}
