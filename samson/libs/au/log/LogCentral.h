


#ifndef _H_AU_MAIN_LOG_CENTRAL
#define _H_AU_MAIN_LOG_CENTRAL

#include <set>

#include "au/Singleton.h"
#include "au/ThreadManager.h"
#include "au/console/CommandCatalogue.h"
#include "au/log/LogChannelFilter.h"
#include "au/log/LogChannels.h"
#include "au/network/FileDescriptor.h"

namespace au {
class Log;
class LogPluginScreen;
class LogPluginFile;
class LogPluginServer;

void *run_LogCentral(void *p);

//
class LogCentral {
public:

  LogCentral();

  // Init log system
  void Init(const std::string& exec = "Unknown");

  // Stop the loggin system
  void Stop();

  // Emit a log thougth the pipe
  void Emit(Log *log);

  // Console interface for this element
  void evalCommand(const std::string& command);
  void evalCommand(const std::string& command, au::ErrorManager& error);

  // Accessors
  inline LogChannels& log_channels();

  // Add and remove plugins to the system
  void AddPlugin(const std::string& name,  LogPlugin *p) {
    au::ErrorManager error;

    AddPlugin(name, p, error);
  }

  void AddPlugin(const std::string& name,  LogPlugin *p, au::ErrorManager& error);

  inline bool CheckLogChannel(int c) {
    return main_log_channel_filter_.IsChannelActivated(c);
  }

  inline LogChannelFilter& main_log_channel_filter() {
    return main_log_channel_filter_;
  }

  //Return the file descriptor used to send traces
  int log_fd() const
  {
    return fds_[1];
  }
  
private:

  // Main function for the background thread
  void run();

  // Review if channles are activated
  void ReviewChannelsActivateion();


  // Channel registration
  LogChannels log_channels_;

  // Frind function to run in background
  friend void *run_LogCentral(void *p);


  // Flag to indicate the backgroud thread to finish
  bool quit_;

  // Pipe and thread for the log process
  pthread_t t_;
  int fds_[2];

  // File descriptor to emit logs
  au::FileDescriptor *fd_write_logs_;
  au::FileDescriptor *fd_read_logs_;

  // Name of the main executalbe
  std::string exec_;

  // Set of Plugins for logs
  au::map<std::string, LogPlugin> plugins_;

  // Main elements to emit or not logs
  LogChannelFilter main_log_channel_filter_;

  // Counter of logs
  LogCounter log_counter_;
};

extern LogCentral log_central;


class LogCentralCatalogue : public au::console::CommandCatalogue {
public:

  // Constructor with commands definitions
  LogCentralCatalogue();
};
}

#endif  // ifndef _H_AU_MAIN_LOG_CENTRAL