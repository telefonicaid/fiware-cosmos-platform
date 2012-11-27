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

#include "au/log/LogCentral.h"  // Own interface

#include <assert.h>

#include "au/ThreadManager.h"
#include "au/console/CommandCatalogue.h"
#include "au/log/LogCentralChannels.h"
#include "au/log/LogCentralChannelsFilter.h"
#include "au/log/LogCentralPluginFile.h"
#include "au/log/LogCentralPluginScreen.h"
#include "au/log/LogCentralPluginServer.h"
#include "au/log/LogCommon.h"
#include "au/network/FileDescriptor.h"
#include "au/singleton/Singleton.h"

namespace au {
// Global instance of LogCentral
LogCentral *log_central = NULL;

void LogCentral::InitLogSystem(const std::string& exec_name) {
  StopLogSystem();   // Just in case it was previously added
  log_central = new LogCentral();
  log_central->Init(exec_name);
}

void LogCentral::StopLogSystem() {
  if (log_central) {
    log_central->Stop();
    delete log_central;
    log_central = NULL;
  }
}

LogCentral *LogCentral::Shared() {
  return log_central;
}

LogCentral::LogCentral() : au::Thread("LogCentral") {
  fds_[0] = -1;
  fds_[1] = -1;

  fd_read_logs_ = NULL;
  fd_write_logs_ = NULL;

  node_ = "Unknown";  // No default name for this
}

void LogCentral::AddFilePlugin(const std::string& plugin_name, const std::string& file_name) {
  evalCommand("log_to_file " + file_name  + " -name " + plugin_name);
}

void LogCentral::AddScreenPlugin(const std::string& plugin_name, const std::string& format) {
  evalCommand("log_to_screen -name " + plugin_name + " -format \"" + format + "\"");
}

void LogCentral::RemovePlugin(const std::string& plugin_name) {
  LogCentralPlugin *plugin = plugins_.extractFromMap(plugin_name);

  if (plugin) {
    delete plugin;
  }
}

void LogCentral::AddServerPlugin(const std::string& plugin_name, const std::string& host,
                                 const std::string file_name) {
  evalCommand("log_to_server " + host + " " + file_name  + " -name " + plugin_name);
}

std::string LogCentral::GetPluginStatus(const std::string& name) {
  LogCentralPlugin *plugin = plugins_.findInMap(name);

  if (!plugin) {
    return "-";
  } else {
    return plugin->status();
  }
}

std::string LogCentral::GetPluginChannels(const std::string& name) {
  LogCentralPlugin *plugin = plugins_.findInMap(name);

  if (!plugin) {
    return "-";
  } else {
    return plugin->log_channel_filter().description();
  }
}

void LogCentral::AddPlugin(const std::string& name,  LogCentralPlugin *p) {
  au::ErrorManager error;

  AddPlugin(name, p, error);
}

void LogCentral::AddPlugin(const std::string& name, LogCentralPlugin *log_plugin, au::ErrorManager& error) {
  if (plugins_.findInMap(name) != NULL) {
    error.set(au::str("Plugin %s already exists", name.c_str()));
    return;   // Plugin already included with this name
  }
  plugins_.insertInMap(name, log_plugin);
}

void LogCentral::CreatePipeAndFileDescriptors() {
  if (fd_write_logs_ != NULL) {
    fd_write_logs_->Close();
    fd_write_logs_ = NULL;
  }
  if (fd_read_logs_ != NULL) {
    fd_read_logs_->Close();
    fd_read_logs_ = NULL;
  }

  int r = pipe(fds_);
  if (r != 0) {
    fprintf(stderr, "Not possible to create pipe for logs\n");
    exit(1);
  }

  // Create file descriptor to write logs
  fd_write_logs_ = new au::FileDescriptor("fd for writting logs", fds_[1]);
  fd_read_logs_ = new au::FileDescriptor("fd for reading logs", fds_[0]);
}

void LogCentral::Init(const std::string& exec) {
  // keep the name of the executable
  exec_ = exec;

  if (fd_read_logs_) {
    return;  // Already initialized
  }
  CreatePipeAndFileDescriptors();
  StartThread();  // Start background thread to handle logs
}

void LogCentral::Stop() {
  // Close write file descriptor
  if (fd_write_logs_ != NULL) {
    fd_write_logs_->Close();
    fd_write_logs_ = NULL;
  }

  // Stop background thread
  StopThread();

  // Close read pipe for logs
  if (fd_read_logs_ != NULL) {
    fd_read_logs_->Close();
    fd_read_logs_ = NULL;
  }

  // Remove plugins
  plugins_.clearMap();

  // Reset channels registered so far
  log_channels_.Clear();
  ReviewChannelsLevels();

  // Clear counter of logs
  log_counter_.Clear();
}

void LogCentral::Flush() {
  // Close write file descriptor
  if (fd_write_logs_ != NULL) {
    fd_write_logs_->Close();
    fd_write_logs_ = NULL;
  }

  // Stop background thread
  StopThread();

  // Close read pipe for logs
  if (fd_read_logs_ != NULL) {
    fd_read_logs_->Close();
    fd_read_logs_ = NULL;
  }

  CreatePipeAndFileDescriptors();
  StartThread();    // Start background thread to handle logs
}

void LogCentral::Pause() {
  // Close write file descriptor
  if (fd_write_logs_ != NULL) {
    fd_write_logs_->Close();
    fd_write_logs_ = NULL;
  }

  // Stop background thread
  StopThread();

  // Close read pipe for logs
  if (fd_read_logs_ != NULL) {
    fd_read_logs_->Close();
    fd_read_logs_ = NULL;
  }
}

void LogCentral::Play() {
  CreatePipeAndFileDescriptors();
  StartThread();    // Start background thread to handle logs
}

void LogCentral::Emit(Log *log) {
  // Write to the pipe
  if (fd_write_logs_ == NULL) {
    return;
  }
  log->Write(fd_write_logs_);
}

void LogCentral::RunThread() {
  // Background thread
  while (true) {
    LogPointer log(new Log());
    bool real_log = log->Read(fd_read_logs_);

    if (!real_log) {
      if (IsThreadQuiting()) {
        return;  // Finish this thread if I am suppoused to do so
      }
      continue;
    }

    // Additional information for logs
    int channel = log->log_data().channel;
    log->Set("channel_name", log_channels_.channel_name(channel));
    log->Set("exec",  exec_);
    log->Set("node",  node_);

    // Total count of logs
    log_counter_.Process(log);

    // Process log to different plugins
    au::map<std::string, LogCentralPlugin>::iterator it;
    for (it = plugins_.begin(); it != plugins_.end(); it++) {
      LogCentralPlugin *log_plugin = it->second;
      if (log_plugin->IsLogAccepted(log)) {
        log_plugin->Process(log);
      }
    }
  }
}

void LogCentral::ReviewChannelsLevels() {
  for (int c = 0; c < LOG_MAX_CHANNELS; c++) {
    int max_level = 0;

    if (log_channels_.IsRegistered(c)) {
      au::map<std::string, LogCentralPlugin>::iterator it;
      for (it = plugins_.begin(); it != plugins_.end(); it++) {
        int level = it->second->log_channel_filter().GetLevel(c);
        if (level > max_level) {
          max_level = level;
        }
      }
    }
    main_log_channel_filter_.SetLevel(c, max_level);
  }
}

void LogCentral::evalCommand(const std::string& command) {
  au::ErrorManager error;

  evalCommand(command, error);
}

void LogCentral::evalCommand(const std::string& command, au::ErrorManager& error) {
  // Catalogue to parse input commands ( separated by commas )
  LogCentralCatalogue log_central_catalogue;

  // We allo to receive several commands separated by a ","
  std::vector<std::string> commands = au::split(command, ',');
  for (size_t i = 0; i < commands.size(); i++) {
    // Get instance of the command (using command catalogue )
    au::console::CommandInstance *command_instance = log_central_catalogue.parse(command, error);

    // If error parsing the command, just return the error
    if (error.IsActivated()) {
      return;
    }

    if (command_instance->main_command() == "log_help") {
      error.AddMessage(log_central_catalogue.getHelpForConcept("all"));
      return;
    }

    // "show_fields"
    if (command_instance->main_command() == "log_show_fields") {
      error.AddMessage(au::getTableOfFields()->str());
      return;
    }

    if (command_instance->main_command() == "log_show_plugins") {
      au::tables::Table table("Name|Active|Total logs|Total Size|Rate|Status,left");
      table.setTitle("Log Plugins");

      au::map<std::string, LogCentralPlugin>::iterator it;
      for (it = plugins_.begin(); it != plugins_.end(); it++) {
        LogCentralPlugin *log_plugin = it->second;

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
      AddPlugin(name, new LogCentralPluginScreen(format), error);
      error.AddMessage(au::str("Added a plugin to send logs to screen"));
      return;
    }

    if (command_instance->main_command() == "log_to_file") {
      std::string name = command_instance->get_string_option("name");
      std::string filename = command_instance->get_string_argument("filename");
      AddPlugin(name, new LogCentralPluginFile(filename), error);
      error.AddMessage(au::str("Added a plugin to send logs to file %s", filename.c_str()));
      return;
    }

    if (command_instance->main_command() == "log_to_server") {
      std::string name = command_instance->get_string_option("name");
      std::string filename = command_instance->get_string_argument("filename");
      std::string host = command_instance->get_string_argument("host");
      AddPlugin(name, new LogCentralPluginServer(host, LOG_SERVER_DEFAULT_PORT, filename), error);
      error.AddMessage(au::str("Added a plugin to send logs to server %s", host.c_str()));
      return;
    }

    // Add or remove channels

    if (command_instance->main_command() == "log_set") {
      std::string channel_pattern_string = command_instance->get_string_argument("channel_pattern");
      std::string str_log_level = command_instance->get_string_argument("log_level");
      std::string plugin_pattern_string = command_instance->get_string_argument("plugin_pattern");
      int log_level = Log::GetLogLevel(str_log_level);

      au::SimplePattern channel_pattern(channel_pattern_string);
      au::SimplePattern plugin_pattern(plugin_pattern_string);

      // Count elements
      bool no_elements = true;

      for (int c = 0; c < LOG_MAX_CHANNELS; c++) {  // Loop channels
        if (!log_channels_.IsRegistered(c)) {
          continue;
        }

        std::string channel_name = log_channels_.channel_name(c);
        if (!channel_pattern.match(channel_name)) {
          continue;
        }

        au::map<std::string, LogCentralPlugin>::iterator it;  // Loop plugins
        for (it = plugins_.begin(); it != plugins_.end(); it++) {
          std::string plugin_name = it->first;

          if (plugin_pattern.match(plugin_name)) {
            LogCentralChannelsFilter& log_channel_filter = it->second->log_channel_filter();
            error.AddMessage(au::str("Plugin %s : Channel %s is activated for this log level (%s)"
                                     , plugin_name.c_str()
                                     , channel_name.c_str()
                                     , str_log_level.c_str()));
            no_elements = false;
            log_channel_filter.SetLevel(c, log_level);
          }
        }
      }

      // Review channels
      if (no_elements) {
        error.AddWarning("No channel updated in any plugin ");
      } else {
        ReviewChannelsLevels();
      }
      return;
    }

    if (command_instance->main_command() == "log_add") {
      std::string channel_pattern_string = command_instance->get_string_argument("channel_pattern");
      std::string str_log_level = command_instance->get_string_argument("log_level");
      std::string plugin_pattern_string = command_instance->get_string_argument("plugin_pattern");
      int log_level = Log::GetLogLevel(str_log_level);

      au::SimplePattern channel_pattern(channel_pattern_string);
      au::SimplePattern plugin_pattern(plugin_pattern_string);

      // Count elements
      bool no_elements = true;

      for (int c = 0; c < LOG_MAX_CHANNELS; c++) {  // Loop channels
        if (!log_channels_.IsRegistered(c)) {
          continue;
        }

        std::string channel_name = log_channels_.channel_name(c);
        if (!channel_pattern.match(channel_name)) {
          continue;
        }

        au::map<std::string, LogCentralPlugin>::iterator it;  // Loop plugins
        for (it = plugins_.begin(); it != plugins_.end(); it++) {
          std::string plugin_name = it->first;

          if (plugin_pattern.match(plugin_name)) {
            LogCentralChannelsFilter& log_channel_filter = it->second->log_channel_filter();
            int current_level = log_channel_filter.GetLevel(c);
            if (current_level >= log_level) {
              error.AddWarning(au::str("Plugin %s : Channel %s is already activated for this log level"
                                       , plugin_name.c_str()
                                       , channel_name.c_str()
                                       , str_log_level.c_str()));
            } else {
              error.AddMessage(au::str("Plugin %s : Channel %s is activatd for this log level (%s)"
                                       , channel_name.c_str()
                                       , it->first.c_str()
                                       , str_log_level.c_str()));
              no_elements = false;
              log_channel_filter.SetLevel(c, log_level);
            }
          }
        }
      }

      // Review channels
      if (no_elements) {
        error.AddWarning("No channel updated in any plugin ");
      } else {
        ReviewChannelsLevels();
      }
      return;
    }

    if (command_instance->main_command() == "log_remove") {
      std::string channel_pattern_string = command_instance->get_string_argument("channel_pattern");
      std::string plugin_pattern_string = command_instance->get_string_argument("plugin_pattern");

      au::SimplePattern channel_pattern(channel_pattern_string);
      au::SimplePattern plugin_pattern(plugin_pattern_string);

      // Count elements
      bool no_elements = true;

      for (int c = 0; c < LOG_MAX_CHANNELS; c++) {  // Loop channels
        if (!log_channels_.IsRegistered(c)) {
          continue;
        }

        std::string channel_name = log_channels_.channel_name(c);
        if (!channel_pattern.match(channel_name)) {
          continue;
        }

        au::map<std::string, LogCentralPlugin>::iterator it;  // Loop plugins
        for (it = plugins_.begin(); it != plugins_.end(); it++) {
          std::string plugin_name = it->first;

          if (plugin_pattern.match(plugin_name)) {
            LogCentralChannelsFilter& log_channel_filter = it->second->log_channel_filter();
            int current_level = log_channel_filter.GetLevel(c);

            if (current_level == 0) {
              error.AddWarning(au::str("Plugin %s: Channel %s : logs already deativated "
                                       , plugin_name.c_str()
                                       , channel_name.c_str()
                                       ));
            } else {
              error.AddMessage(au::str("Plugin %s : Channel %s : logs are deactivated"
                                       , channel_name.c_str()
                                       , it->first.c_str()
                                       ));
              no_elements = false;
              log_channel_filter.SetLevel(c, 0);
            }
          }
        }
      }

      // Review channels
      if (no_elements) {
        error.AddWarning("No channel updated in any plugin ");
      } else {
        ReviewChannelsLevels();
      }
      return;
    }


    if (command_instance->main_command() == "log_show_channels") {
      bool rates = command_instance->get_bool_option("rates");
      bool verbose = command_instance->get_bool_option("v");

      std::string table_definition = "Channel,left|Level|Count";

      au::map<std::string, LogCentralPlugin>::iterator it;
      for (it = plugins_.begin(); it != plugins_.end(); it++) {
        table_definition += ("|" + it->first + " (" +  it->second->str_info() + ")" + ",left");
      }
      table_definition += "|Description,left";

      au::tables::Table table(table_definition);
      table.setTitle("Channels for logging");

      for (int i =  0; i < log_channels_.num_channels(); i++) {
        std::string name = log_channels_.channel_name(i);
        std::string description = log_channels_.channel_description(i);

        au::StringVector values;
        values.Push(name);
        values.Push(Log::GetLogLevel(main_log_channel_filter_.GetLevel(i)));

        if (rates) {
          values.Push(log_counter_.str_rate(i));
        } else if (verbose) {
          values.Push(log_counter_.str_types_per_channel(i));
        } else {
          values.Push(log_counter_.str(i));
        }

        au::map<std::string, LogCentralPlugin>::iterator it;
        for (it = plugins_.begin(); it != plugins_.end(); it++) {
          LogCentralPlugin *log_plugin = it->second;

          std::ostringstream info;
          info << "[" << Log::GetLogLevel(log_plugin->log_channel_filter().GetLevel(i)) << "] ";
          if (rates) {
            info << log_plugin->log_counter().str_rate(i);
          } else if (verbose) {
            info << log_plugin->log_counter().str_types_per_channel(i);
          } else {
            info << log_plugin->log_counter().str(i);
          }

          values.Push(info.str());
        }

        values.Push(description);
        table.addRow(values);
      }

      error.AddMessage(table.str());
      return;
    }
  }
}

LogCentralCatalogue::LogCentralCatalogue() {
  add("log_help", "general", "Show help about log_X commands to interact with the log system");

  add("log_to_screen", "general", "Add a log plugin to emit logs to screen");
  add_string_option("log_to_screen", "-name", "screen", "Name of the plugin");
  add_string_option("log_to_screen", "-format", LOG_DEFAULT_FORMAT, "Format of logs on screen");

  add("log_to_file", "general", "Add a log plugin to emit logs to a file");
  add_mandatory_string_argument("log_to_file", "filename", "File to be created to store logs");
  add_string_option("log_to_file", "name", "file", "Name of the plugin");

  add("log_to_server", "general", "Add a log plugin to emit logs to a server");
  add_mandatory_string_argument("log_to_server", "host", "Host of the log server");
  add_mandatory_string_argument("log_to_server", "filename",
                                "File to be created to store logs when connection is not available");
  add_string_option("log_to_server", "name", "server", "Name of the plugin");

  // Show information
  add("log_show_fields", "general", "Show available fields for logs");
  add("log_show_plugins", "general", "Show current plugins for logs");
  add("log_show_channels", "general", "Show logs generatd for all channels and all plugins");
  add_bool_option("log_show_channels", "rates", "Show information about logs generated in bytes/second");
  add_bool_option("log_show_channels", "v", "Show more information about generated logs");

  // Set level of log messages for a particular channel
  add("log_set", "general",
      "Set log-level to specified value for some log-channels ( and for some log-plugins if specified )");
  add_mandatory_string_argument("log_set", "channel_pattern", "Name (or pattern) of log channel");
  add_mandatory_string_argument("log_set", "log_level", "Level of log D,M,V,V2,V3,V4,V5,W,E,X ( type - for no log )");
  add_string_argument("log_set", "plugin_pattern", "*", "Name (or pattern) of log-plugin (type * or nothing for all)");

  // Set level of log messages for a particular channel
  add("log_add", "general",
      "Set log-level at least to specified value for some log-channels ( and for some log-plugins if specified )");
  add_mandatory_string_argument("log_add", "channel_pattern", "Name (or pattern) of log channel");
  add_mandatory_string_argument("log_add", "log_level", "Level of log D,M,V,V2,V3,V4,V5,W,E,X ( type - for no log )");
  add_string_argument("log_add", "plugin_pattern", "*", "Name (or pattern) of log-plugin (type * or nothing for all)");

  // Set level of log messages for a particular channel
  add("log_remove", "general", "Unset logs for some log-channels ( and for some log-plugins if specified )");
  add_mandatory_string_argument("log_remove", "channel_pattern", "Name (or pattern) of log channel");
  add_string_argument("log_remove", "plugin_pattern", "*",
                      "Name (or pattern) of log-plugin (type * or nothing for all)");
}
}
