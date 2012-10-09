
#include "LogCommon.h"
#include "au/log/LogPluginFile.h"
#include "au/log/LogPluginScreen.h"
#include "au/log/LogPluginServer.h"

#include "LogCentral.h"  // Own interface


namespace au {
// Global instance of LogCentral
LogCentral log_central;

void *run_LogCentral(void *p) {
  LogCentral *log_central = (LogCentral *)p;

  log_central->run();
  return NULL;
}

LogCentral::LogCentral() {
  fds_[0] = -1;
  fds_[1] = -1;

  fd_read_logs_ = NULL;
  fd_write_logs_ = NULL;
  
  quit_ = false;
}

void LogCentral::AddPlugin(const std::string& name, LogPlugin *log_plugin, au::ErrorManager& error) {
  if (plugins_.findInMap(name) != NULL) {
    error.set(au::str("Plugin %s already exists", name.c_str()));
    return;   // Plugin already included with this name
  }
  plugins_.insertInMap(name, log_plugin);
}

inline LogChannels& LogCentral::log_channels() {
  return log_channels_;
}

void LogCentral::Init(std::string exec) {

  // Flag to indicate background thread to quit
  quit_ = false;

  // keep the name of the executable
  exec_ = exec;

  if (fds_[0] != -1) {
    // Already init
    return;
  }

  int r = pipe(fds_);
  if (r != 0) {
    fprintf(stderr, "Not possible to create pipe for logs\n");
    exit(1);
  }

  // Create file descriptor to write logs
  fd_write_logs_ = new au::FileDescriptor("fd for writting logs", fds_[1]);
  fd_read_logs_ = new au::FileDescriptor("fd for reading logs", fds_[0]);

  // Create background process for logs
  au::ThreadManager *tm = au::Singleton<au::ThreadManager>::shared();
  tm->addThread("log_thread", &t_, NULL, run_LogCentral, this);
}

  void LogCentral::Stop()
  {
    // Flush all pending logs
    // Stop the background thread

    // Set the quit flag
    quit_ = true;
    
    // Close pipes to force log process to fail
    close(fds_[0]);
    close(fds_[1]);

    // Wait for the background threads
    void* return_code; // Return code to be ignored
    pthread_join(t_, &return_code);
    
  }

  
void LogCentral::Emit(Log *log) {
  // Write to the pipe
  log->Write(fd_write_logs_);
}

void LogCentral::run() {
  // Background thread

  while (true) {
    LogPointer log(new Log());
    bool real_log = log->Read(fd_read_logs_);

    if (!real_log) {
      if( quit_)
        return; // Finish this thread
      continue;
    }

    // Additional information for logs
    log->Set("channel_name", log_channels_.channel_name(log->log_data().channel));
    log->Set("channel_alias", log_channels_.channel_alias(log->log_data().channel));
    log->Set("exec",  exec_);


    // Total count of logs
    log_counter_.Process(log);

    // Process log to different plugins
    au::map<std::string, LogPlugin>::iterator it;
    for (it = plugins_.begin(); it != plugins_.end(); it++) {
      LogPlugin *log_plugin = it->second;
      if (log_plugin->Accept(log)) {
        log_plugin->Process(log);
      }
    }
  }
}

void LogCentral::ReviewChannelsActivateion() {
  for (int c = 0; c < AU_LOG_MAX_CHANNELS; c++) {
    bool activated = false;   // Only activated if a plugin is interested

    if (log_channels_.IsRegistered(c)) {
      // Go to all plugins...
      au::map<std::string, LogPlugin>::iterator it;
      for (it = plugins_.begin(); it != plugins_.end(); it++) {
        if (it->second->log_channel_filter().IsChannelActivated(c)) {
          activated = true;
        }
      }
    }

    main_log_channel_filter_.Set(c, activated);
  }
}

void LogCentral::evalCommand(std::string command) {
  au::ErrorManager error;

  evalCommand(command, error);
}

void LogCentral::evalCommand(std::string command, au::ErrorManager& error) {
  // Catalogue to parse input commands ( separated by commas )
  LogCentralCatalogue log_central_catalogue;

  std::vector<std::string> commands = au::split(command, ',');
  for (size_t i = 0; i < commands.size(); i++) {
    au::console::CommandInstance *command_instance = log_central_catalogue.parse(command, error);

    if (error.IsActivated()) {
      return;
    }

    // "show_fields"
    if (command_instance->main_command() == "show_fields") {
      error.AddMessage(getTableOfFields()->str());
      return;
    }

    if (command_instance->main_command() == "show_plugins") {
      au::tables::Table table("Name|Active|Total logs|Total Size|Rate|Status,left");

      au::map<std::string, LogPlugin>::iterator it;
      for (it = plugins_.begin(); it != plugins_.end(); it++) {
        LogPlugin *log_plugin = it->second;

        au::StringVector values;

        values.Push(it->first);
        values.Push(log_plugin->activated() ? "Yes" : "No");
        values.Push(au::str(log_plugin->log_counter().logs()).c_str());
        values.Push(au::str(log_plugin->log_counter().size(), "B").c_str());
        values.Push(au::str(log_plugin->log_counter().rate(), "B/s").c_str());

        values.Push(log_plugin->status());

        table.addRow(values);
      }

      error.AddMessage(table.str());
      return;
    }


    // screen command
    if (command_instance->main_command() == "log_to_screen") {
      std::string name = command_instance->get_string_option("name");
      std::string format = command_instance->get_string_option("format");
      AddPlugin(name, new LogPluginScreen(format), error);
      error.AddMessage(au::str("Added a plugin to send logs to screen"));
      return;
    }

    if (command_instance->main_command() == "log_to_file") {
      std::string name = command_instance->get_string_option("name");
      std::string filename = command_instance->get_string_argument("filename");
      AddPlugin(name, new LogPluginFile(filename), error);
      error.AddMessage(au::str("Added a plugin to send logs to file %s", filename.c_str()));
      return;
    }

    if (command_instance->main_command() == "log_to_server") {
      std::string name = command_instance->get_string_option("name");
      std::string filename = command_instance->get_string_argument("filename");
      std::string host = command_instance->get_string_argument("host");
      AddPlugin(name, new LogPluginServer(host, AU_LOG_SERVER_PORT, filename), error);
      error.AddMessage(au::str("Added a plugin to send logs to server %s", host.c_str()));
      return;
    }

    // Add or remove channels
    // add channel_name plugin_patter

    if (command_instance->main_command() == "add") {
      std::string channel_pattern_string = command_instance->get_string_argument("channel_pattern");
      std::string plugin_pattern_string = command_instance->get_string_argument("plugin_pattern");

      au::SimplePattern channel_pattern(channel_pattern_string);
      au::SimplePattern plugin_pattern(plugin_pattern_string);

      // Count elements
      bool no_elements = true;

      for (int c = 0; c < AU_LOG_MAX_CHANNELS; c++) {
        if (!log_channels_.IsRegistered(c)) {
          continue;
        }

        std::string channel_name = log_channels_.channel_name(c);

        if (!channel_pattern.match(channel_name)) {
          continue;
        }

        // Go to all plugins...
        au::map<std::string, LogPlugin>::iterator it;
        for (it = plugins_.begin(); it != plugins_.end(); it++) {
          if (plugin_pattern.match(it->first)) {
            if (it->second->CheckLogChannel(c)) {
              error.AddWarning(au::str("Channel %s is already activated for plugin %s", channel_name.c_str(),
                                       it->first.c_str()));
            } else {
              error.AddMessage(au::str("Adding channel %s for plugin %s", channel_name.c_str(), it->first.c_str()));
              no_elements = false;
              it->second->log_channel_filter().Add(c);
            }
          }
        }
      }

      // Review channels
      if (no_elements) {
        error.AddMessage("No channel updated");
      } else {
        ReviewChannelsActivateion();
      }
      return;
    }

    if (command_instance->main_command() == "remove") {
      std::string channel_pattern_string = command_instance->get_string_argument("channel_pattern");
      std::string plugin_pattern_string = command_instance->get_string_argument("plugin_pattern");

      au::SimplePattern channel_pattern(channel_pattern_string);
      au::SimplePattern plugin_pattern(plugin_pattern_string);

      // Count elements
      bool no_elements = true;

      for (int c = 0; c < AU_LOG_MAX_CHANNELS; c++) {
        if (!log_channels_.IsRegistered(c)) {
          continue;
        }

        std::string channel_name = log_channels_.channel_name(c);

        if (!channel_pattern.match(channel_name)) {
          continue;
        }

        // Go to all plugins...
        au::map<std::string, LogPlugin>::iterator it;
        for (it = plugins_.begin(); it != plugins_.end(); it++) {
          if (plugin_pattern.match(it->first)) {
            if (!it->second->CheckLogChannel(c)) {
              error.AddWarning(au::str("Channel %s is already deactivated for plugin %s", channel_name.c_str(),
                                       it->first.c_str()));
            } else {
              error.AddMessage(au::str("Removing channel %s for plugin %s", channel_name.c_str(), it->first.c_str()));
              no_elements = false;
              it->second->log_channel_filter().Remove(c);
            }
          }
        }
      }

      // Review channels
      if (no_elements) {
        error.AddMessage("No channel updated");
      } else {
        ReviewChannelsActivateion();
      }
      return;
    }

    if (command_instance->main_command() == "verbose") {
      main_log_channel_filter_.Add(AU_LOG_V);
    }
    if (command_instance->main_command() == "verbose2") {
      main_log_channel_filter_.Add(AU_LOG_V);
      main_log_channel_filter_.Add(AU_LOG_V2);
    }
    if (command_instance->main_command() == "verbose3") {
      main_log_channel_filter_.Add(AU_LOG_V);
      main_log_channel_filter_.Add(AU_LOG_V2);
      main_log_channel_filter_.Add(AU_LOG_V3);
    }
    if (command_instance->main_command() == "verbose3") {
      main_log_channel_filter_.Add(AU_LOG_V);
      main_log_channel_filter_.Add(AU_LOG_V2);
      main_log_channel_filter_.Add(AU_LOG_V3);
      main_log_channel_filter_.Add(AU_LOG_V4);
    }
    if (command_instance->main_command() == "verbose3") {
      main_log_channel_filter_.Add(AU_LOG_V);
      main_log_channel_filter_.Add(AU_LOG_V2);
      main_log_channel_filter_.Add(AU_LOG_V3);
      main_log_channel_filter_.Add(AU_LOG_V4);
      main_log_channel_filter_.Add(AU_LOG_V4);
    }


    if (command_instance->main_command() == "show_channels") {
      bool rates = command_instance->get_bool_option("rates");

      std::string table_definition = "Alias|Channel|Active|Count";

      au::map<std::string, LogPlugin>::iterator it;
      for (it = plugins_.begin(); it != plugins_.end(); it++) {
        table_definition += ( "|" + it->first + " (" +  it->second->str_info() + ")" + ",left" );
      }

      au::tables::Table table(table_definition);
      table.setTitle("Channels for logging");

      for (int i =  0; i < AU_LOG_MAX_CHANNELS; i++) {
        std::string name = log_channels_.channel_name(i);
        if (name != "") {
          au::StringVector values;
          values.Push(log_channels_.channel_alias(i));
          values.Push(name);
          values.Push(main_log_channel_filter_.IsChannelActivated(i) ? "[Y]" : "[N]");

          if (rates) {
            values.Push(log_counter_.str_rate(i));
          } else {
            values.Push(log_counter_.str(i));
          }

          au::map<std::string, LogPlugin>::iterator it;
          for (it = plugins_.begin(); it != plugins_.end(); it++) {
            std::ostringstream info;
            LogPlugin *log_plugin = it->second;

            if (log_plugin->log_channel_filter().IsChannelActivated(i)) {
              info << "[Y] ";
            } else {
              info << "[N] ";
            }

            if (rates) {
              info << log_plugin->log_counter().str_rate(i);
            } else {
              info << log_plugin->log_counter().str(i);
            }

            values.Push(info.str());
          }

          table.addRow(values);
        }
      }

      error.AddMessage(table.str());
      return;
    }
  }
}

LogCentralCatalogue::LogCentralCatalogue() {
  add("log_to_screen", "general", "Add a log plugin to emit logs to screen");
  add_string_option("log_to_screen", "-name", "screen", "Name of the plugin");
  add_string_option("log_to_screen", "-format", AU_LOG_DEFAULT_FORMAT, "Format of logs on screen");

  add("log_to_file", "general", "Add a log plugin to emit logs to a file");
  add_mandatory_string_argument("log_to_file", "filename", "File to be created to store logs");
  add_string_option("log_to_file", "name", "file", "Name of the plugin");

  add("log_to_server", "general", "Add a log plugin to emit logs to a server");
  add_mandatory_string_argument("log_to_server", "host", "Host of the log server");
  add_mandatory_string_argument("log_to_server", "filename",
                                "File to be created to store logs when connection is not available");
  add_string_option("log_to_server", "name", "server", "Name of the plugin");

  // Show information
  add("show_fields", "general", "Show available fields for logs");
  add("show_plugins", "general", "Show current plugins for logs");
  add("show_channels", "general", "Show current channels and all plugins activation");
  add_bool_option("show_channels", "-rates", "Show information about data generated in bytes/second");

  // Add and remove channels for logging
  add("add", "general", "Add a channel to generate logs");
  add_mandatory_string_argument("add", "channel_pattern", "Name (or pattern) of log channel");
  add_string_argument("add", "plugin_pattern", "*", "Name (or pattern) of plugin channel");

  add("remove", "general", "Remove a channel to generate logs");
  add_mandatory_string_argument("remove", "channel_pattern", "Name (or pattern) of log channel");
  add_string_argument("remove", "plugin_pattern", "*", "Name (or pattern) of plugin channel");

  add("verbose", "general", "Add verbose channel to generate logs");
  add("verbose2", "general", "Add verbose-level-2 channel to generate logs");
  add("verbose3", "general", "Add verbose-level-3 channel to generate logs");
}
}