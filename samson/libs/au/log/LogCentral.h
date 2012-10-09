
#ifndef _H_AU_MAIN_LOG_CENTRAL
#define _H_AU_MAIN_LOG_CENTRAL

#include <set>

#include "au/console/CommandCatalogue.h"
#include "au/log/LogChannelFilter.h"
#include "au/log/LogChannels.h"
#include "au/log/LogDispatcher.h"
#include "au/network/FileDescriptor.h"
#include "au/Singleton.h"
#include "au/ThreadManager.h"

namespace au {

class Log;
class LogPluginScreen;
class LogPluginFile;
class LogPluginServer;

void* run_LogCentral(void* p);

//
class LogCentral {

  public:

    LogCentral();

    // Init log system
    void Init(std::string exec = "Unknown");

    // Emit a log thougth the pipe
    void Emit(Log* log);

    // Console interface for this element
    void evalCommand(std::string command);
    void evalCommand(std::string command, au::ErrorManager& error);

    // Accessors
    inline LogChannels& log_channels();

    // Add and remove plugins to the system
    void AddPlugin(const std::string& name, LogPlugin *p);

    inline bool CheckLogChannel(int c) {
      return main_log_channel_filter_.IsChannelActivated(c);
    }

    inline LogChannelFilter& main_log_channel_filter() {
      return main_log_channel_filter_;
    }

    void stop() {
      marked_to_stop_ = true;
      evalCommand("ping");
    }

  private:

    // MAin function for the background thread
    void run();

    // Channel registration
    LogChannels log_channels_;

    // Friend function to run in background
    friend void* run_LogCentral(void* p);

    // Pipe and thread for the log process
    pthread_t t_;
    int fds_[2];

    // File descriptor to emit logs
    au::FileDescriptor *fd_write_logs_;
    au::FileDescriptor *fd_read_logs_;

    // Fix plugins
    LogPluginScreen* screen_plugin_;
    LogPluginFile* file_plugin_;
    LogPluginServer* server_plugin_;

    // Name of the main executalbe
    std::string exec_;

    // Set of Plugins for logs
    au::map<std::string, LogPlugin> plugins_;

    // Main elements to emit or not logs
    LogChannelFilter main_log_channel_filter_;

    volatile bool marked_to_stop_;

};

extern LogCentral log_central;

class LogCentralCatalogue : public au::console::CommandCatalogue {
  public:

    // Constructor with commands definitions
    LogCentralCatalogue() {
      add("screen", "general", "Mofify log to screen setup");
      add_mandatory_string_argument("screen", "command", "on/off activate or deactivate screen log");
      add_string_argument("screen", "format", AU_LOG_DEFAULT_FORMAT, "Format of logs on screen");

      add("file", "general", "Mofify log to file setup");
      add_mandatory_string_argument("file", "command", "on/off activate or deactivate file log");
      add_string_argument("file", "file", "", "File to store logs");

      // test
      add("ping", "general", "Force the logCentral to receive and process a message");

      // Show information
      add("show_fields", "general", "Show available fields for logs");
      add("show_plugins", "general", "Show current plugins for logs");

      // Add and remove channels for logging
      add("add", "general", "Add a channel to generate logs");
      add_mandatory_string_argument("add", "pattern", "Name (or pattern) of log channels");

      add("remove", "general", "Remove a channel to generate logs");
      add_mandatory_string_argument("remove", "pattern", "Name (or pattern) of log channels");

      add("verbose", "general", "Add verbose channel to generate logs");
      add("verbose2", "general", "Add verbose-level-2 channel to generate logs");
      add("verbose3", "general", "Add verbose-level-3 channel to generate logs");

    }
};

}

#endif
