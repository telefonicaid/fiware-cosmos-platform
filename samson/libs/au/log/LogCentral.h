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



#ifndef _H_AU_MAIN_LOG_CENTRAL
#define _H_AU_MAIN_LOG_CENTRAL

#include <set>

#include "au/ThreadManager.h"
#include "au/console/CommandCatalogue.h"
#include "au/log/LogCentralChannels.h"
#include "au/log/LogCentralChannelsFilter.h"
#include "au/network/FileDescriptor.h"
#include "au/singleton/Singleton.h"

namespace au {
class Log;
class LogCentralPluginScreen;
class LogCentralPluginFile;
class LogCentralPluginServer;

/**
 *
 * LogCentral
 *
 * \brief Central element to emit logs using a secondary thread connected with a pipe
 *
 * Note: This strategy allow to receive logs from fork-generated children processes without any problem  with mutexes
 *
 */


class LogCentral : public au::Thread {
public:

  LogCentral();

  // General management
  void Init(const std::string& exec = "Unknown");  // Init log system
  void Stop();                                     // Stop the loggin system unregistering all channels
  void Flush();

  void Pause();
  void Play();

  // Set name of this node
  void set_node(const std::string& node) {
    node_ = node;
  }

  // Inline method to quickly check if a log has to be generated
  inline bool IsLogAccepted(int channel, int level) {
    return main_log_channel_filter_.IsLogAccepted(channel, level);
  }

  // Emit a log thougth the pipe
  void Emit(Log *log);

  // Direct Console interface for this element
  void evalCommand(const std::string& command);
  void evalCommand(const std::string& command, au::ErrorManager& error);

  // Plugins management
  void AddPlugin(const std::string& name,  LogCentralPlugin *p);
  void AddPlugin(const std::string& name,  LogCentralPlugin *p, au::ErrorManager& error);
  void RemovePlugin(const std::string& plugin_name);
  void AddFilePlugin(const std::string& plugin_name, const std::string& file_name);
  void AddServerPlugin(const std::string& plugin_name, const std::string& host, const std::string file_name);
  void AddScreenPlugin(const std::string& plugin_name, const std::string& format = LOG_DEFAULT_FORMAT);
  std::string GetPluginStatus(const std::string& name);
  std::string GetPluginChannels(const std::string& name);

  // Accessors
  LogCentralChannels& log_channels() {
    return log_channels_;
  }

  int GetLogChannelLevel(int c) {
    return main_log_channel_filter_.GetLevel(c);
  }

  LogCentralChannelsFilter& main_log_channel_filter() {
    return main_log_channel_filter_;
  }

  int log_fd() const  // Return the file descriptor used to send traces
  {
    return fds_[1];
  }

private:

  friend void *RunLogCentral(void *p);

  // Init pipe and file descriptors to comunicate all threads with background thread to process logs
  void CreatePipeAndFileDescriptors();

  // Main function for the background thread
  void RunThread();

  // Review if channles are activated
  void ReviewChannelsLevels();

  // Channel registration
  LogCentralChannels log_channels_;

  // Pipe to write and read logs
  int fds_[2];

  // File descriptor to emit logs
  au::FileDescriptor *fd_write_logs_;
  au::FileDescriptor *fd_read_logs_;

  // Name of the main executalbe
  std::string exec_;

  // Name of this node in a distirbuted environment
  std::string node_;

  // Set of Plugins for logs
  au::map<std::string, LogCentralPlugin> plugins_;

  // Main elements to emit or not logs
  LogCentralChannelsFilter main_log_channel_filter_;

  // Counter of logs
  LogCounter log_counter_;
};

extern LogCentral *log_central;

class LogCentralCatalogue : public au::console::CommandCatalogue {
public:

  // Constructor with commands definitions
  LogCentralCatalogue();
};
}

#endif  // ifndef _H_AU_MAIN_LOG_CENTRAL