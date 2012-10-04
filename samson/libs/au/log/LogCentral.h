
#ifndef _H_AU_LOG_CENTRAL
#define _H_AU_LOG_CENTRAL

#include <set>

#include "au/Singleton.h"
#include "au/ThreadManager.h"
#include "au/network/FileDescriptor.h"
#include "au/log/LogChannels.h"
#include "au/log/LogChannelFilter.h"
#include "au/console/CommandCatalogue.h"

namespace au{
  
  class Log;
  class LogPluginScreen;
  class LogPluginFile;
  class LogPluginServer;
  
  class LogCentral
  {
    
  public:
    
    LogCentral();
    
    void Init( std::string exec = "unknown" );
    
    int get_log_fd()
    {
      // Return the fd when logs are written
      return fds_[1];
    }
    
    inline LogChannels& log_channels()
    {
      return log_channels_;
    }
    
    inline LogChannelFilter& main_log_channel_filter()
    {
      return main_log_channel_filter_;
    }
    
    // Emit logs
    inline bool CheckLogChannel( int c )
    {
      return main_log_channel_filter_.IsChannelActivated(c);
    }
    
    // Add and remove plugins to the system
    void AddPlugin(LogPlugin *p);
    void Emit( Log* log );
    
    // Eval command to modify log setup
    void evalCommand(std::string command);
    void evalCommand(std::string command , au::ErrorManager& error );
    
  private:
    
    // Frind function to run in background
    friend   void* run_LogAppServer( void* p );
    
    // Main function for the background thread
    void run();
    
    // Pipe and thread for the log process
    pthread_t t_;
    int fds_[2];
    
    // File descriptor to emit logs
    au::FileDescriptor *fd_write_logs_;
    au::FileDescriptor *fd_read_logs_;
    
    // Channel registration
    LogChannels log_channels_;
    
    // Main elements to emit or not logs
    LogChannelFilter main_log_channel_filter_;
    
    // Set of Plugins for logs
    LogPlugin* plugins[AU_LOG_MAX_PLUGINS];
    
    // Name of the main executalbe
    std::string exec_;
    
    // Fix plugins
    LogPluginScreen* screen_plugin_;
    LogPluginFile* file_plugin_;
    LogPluginServer* server_plugin_;
    
  };
  
  extern LogCentral log_central;
 
  
  class LogCentralCatalogue : public au::console::CommandCatalogue {
  public:
    
    // Constructor with commands definitions
    LogCentralCatalogue()
    {
      add("screen", "general", "Mofify log to screen setup");
      add_mandatory_string_argument("screen", "command", "on/off activate or deactivate screen log");
      add_string_argument("screen","format", AU_LOG_DEFAULT_FORMAT, "Format of logs on screen");

      add("file", "general", "Mofify log to file setup");
      add_mandatory_string_argument("file", "command", "on/off activate or deactivate file log");
      add_string_argument("file","file", "", "File to store logs");

      // Show information
      add("show_fields", "general", "Show available fields for logs");
      add("show_plugins" , "general", "Show current plugins for logs");      
      
      // Add and remove channels for logging
      add("add" , "general", "Add a channel to generate logs");
      add_mandatory_string_argument("add" , "pattern", "Name (or pattern) of log channels");

      add("remove" , "general", "Remove a channel to generate logs");
      add_mandatory_string_argument("remove" , "pattern", "Name (or pattern) of log channels");
      
      add("verbose" , "general", "Add verbose channel to generate logs");
      add("verbose2" , "general", "Add verbose-level-2 channel to generate logs");
      add("verbose3" , "general", "Add verbose-level-3 channel to generate logs");
      
    }
  };

  
}
#endif

