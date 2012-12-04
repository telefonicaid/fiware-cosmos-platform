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
#include "samson/delilah/DelilahCommandCatalogue.h"

namespace samson {
DelilahCommandCatalogue::DelilahCommandCatalogue() {
  // ------------------------------------------------------------------
  // SETUP
  // ------------------------------------------------------------------

  Add("show_cluster_setup", "setup", "Show cluster setup",
      "Get information about workers currently connected to current ( number of cores, memory, etc... )\n");

  Add("show_cluster_assignation", "setup", "Show cluster assignment",
      "Get information about what hash-groups are assigned to what workers\n");

  Add("ls_local_connections", "delilah",
      "Show status of all connections of this delilah ( typically to all workers of the SAMSON cluster )");

  // ------------------------------------------------------------------
  // DELILAH
  // ------------------------------------------------------------------

  Add("connect", "delilah", "Connect to a SAMSON system");
  AddMandatoryStringArgument("connect", "host", "SAMSON node/s to connect (\"host:port host:port..\")");
  AddStringOption("connect", "-digest", "", "Username and password used to connect to this SAMSON cluster");

  Add("disconnect", "delilah", "Disconnect from a SAMSON system");

  Add("alerts", "delilah", "Activate or deactivate showing alerts from SAMSON platform on console");
  AddStringOptionsArgument("alerts", "action", "Options:on:off", "Command to activate or deactivate alerts");

  Add("show_alerts", "delilah", "Show the last alerts received from current SAMSON cluster at this delilah");

  Add("open_alerts_file", "delilah", "Open a local file to store all received alerts");
  AddMandatoryStringArgument("open_alerts_file", "file", "Local file to store alerts");

  Add("close_alerts_file", "delilah", "Close local file opened to store with command 'open_traces_file'");

  Add("verbose", "delilah", "Activate or deactivate verbose mode",
      "In verbose mode, extra information is shown to the user");
  AddStringOptionsArgument("verbose", "action", "Options::on:off");


  Add("send_alert", "delilah", "Send an alert to all connected delilahs using a random worker as sender");
  AddStringOption("send_alert", "-worker", "", "Select broadcaster worker");
  AddBoolOption("send_alert", "-error", "Mark message as an error");
  AddBoolOption("send_alert", "-warning", "Mark message as a warning");
  AddStringArgument("send_alert", "message", "", "Content of the message");

  Add("ps", "delilah", "Show information about delilah processes");
  AddBoolOption("ps", "-clear", "Remove finished processes");
  AddStringArgument("ps", "id", "", "Show more information for this process");

  Add("set", "delilah", "Set environment variable in delilah client");
  AddStringArgument("set", "name", "", "Name of the variable to set");
  AddStringArgument("set", "value", "", "Value to assign");

  Add("unset", "delilah", "Remove an environment variable in delilah client");
  AddStringArgument("set", "name", "", "Name of the variable to set");

  Add("ls_local", "local", "Show a list of current directory with relevant information about local queues");
  AddStringArgument("ls_local", "file", "*", "File or pattern to be displayed");

  Add("ls_local_queues", "local", "Show a list of local queues");
  AddStringArgument("ls_local_queues", "file", "*", "File or pattern to be displayed");

  Add("rm_local", "local", "Remove a local file or directory (and all its contents)");
  AddMandatoryStringArgument("rm_local", "file", "Local file or directory to be removed");

  Add("history", "local", "Show the history of the last commands");
  AddIntOption("history", "limit", 0);

  Add("quit", "delilah", "Quit delilah console");

  // ------------------------------------------------------------------
  // DATA MANIPULATION
  // ------------------------------------------------------------------

  Add("ls", "data", "Show a list of all data queues in the system");
  AddStringArgument("ls", "pattern", "*", "Pattern of the queues to show");
  AddBoolOption("ls", "-a", "Show hidden queues as well ( used internally by the platform )");
  AddBoolOption("ls", "-commit", "Add commit id information for each queue");
  AddBoolOption("ls", "-blocks", "Add #blocks information for each queue");
  AddStringOption("ls", "-group", "", "Group results by a particular column");

  Add("add", "data", "Create a new queue");
  AddMandatoryStringArgument("add", "queue", "Name of the new queue");
  AddMandatoryStringArgument("add", "key_format", "Format for the key's in new queue (txt is accepted)");
  AddStringArgument("add", "value_format", "txt", "Format for the value's in new queue (txt is accepted)");

  Add("remove_all_data", "data", "Remove all queues in the system");

  Add("remove_all", "data", "Remove all queues and stream_operations in the current SAMSON cluster");

  Add("rm", "data", "Remove a queue");
  AddMandatoryStringArgument("rm", "queue", "Queue to be removed");

  Add("set_queue_property", "data", "Specify the value of a property in a queue ");
  AddMandatoryStringArgument("set_queue_property", "queue", "Queue to be updated");
  AddMandatoryStringArgument("set_queue_property", "property", "Name of the property we want to update");
  AddMandatoryStringArgument("set_queue_property", "value", "Value for the selected property");

  Add("unset_queue_property", "data", "Remove a previously defined property for a queue");
  AddMandatoryStringArgument("unset_queue_property", "queue", "Queue to be updated");
  AddMandatoryStringArgument("unset_queue_property", "property", "Name of the property we want to update");

  // ------------------------------------------------------------------
  // Debug
  // ------------------------------------------------------------------

  Add("ls_connections", "debug", "Show status of all connections in the cluster");
  AddUInt64Option("ls_connections", "-w", 0, "Specify a particular worker");
  AddTag("ls_connections", "send_to_all_workers");

  Add("ls_network_queues", "debug", "Show status of all connections in the cluster");
  AddUInt64Option("ls_network_queues", "-w", 0, "Specify a particular worker");
  AddTag("ls_network_queues", "send_to_all_workers");

  Add("threads", "debug", "Show current threads in this delilah");

  Add("ls_blocks", "debug", "Show a list of data blocks managed by SAMSON nodes");
  AddStringArgument("ls_blocks", "pattern", "*", "Pattern to find specific blocks");
  AddUInt64Option("ls_blocks", "-w", static_cast<size_t>(-1), "Specify a worker to request the list of blocks");
  AddTag("ls_blocks", "send_to_all_workers");
  AddBoolOption("ls_blocks", "-info", "Show more information about priority of this block");

  Add("ls_queue_blocks", "debug", "Show detailed list of the blocks included in every queue");
  AddStringArgument("ls_queue_blocks", "pattern", "*", "Pattern to find specific queues");
  AddBoolOption("ls_queue_blocks", "-a", "Show hiden queues as well ( used internally by the platform )");

  Add("ls_buffers", "debug", "Show the list of data buffers managed in a SAMSON cluster. This is a debug tool");
  AddUInt64Option("ls_buffers", "-w", 0, "Specify a particular worker");
  AddTag("ls_buffers", "send_to_all_workers");

  Add("ls_block_requests", "debug", "Show current block requests operations in SAMSON nodes");
  AddStringArgument("ls_block_requests", "pattern", "*", "Select pattern for blocks");
  AddTag("ls_block_requests", "send_to_all_workers");

  Add("ls_block_defrags", "debug", "Show current block defrags operations in all SAMSON nodes");
  AddTag("ls_block_defrags", "send_to_all_workers");

  Add("ls_last_commits", "debug", "Show last commits on data model");

  Add("ls_last_commits_debug", "debug", "Show last commits trials at all workers on data model.");
  AddTag("ls_last_commits_debug", "send_to_all_workers");

  Add("ls_last_tasks", "debug", "Show last 100 tasks scheduled in workers");
  AddBoolOption("ls_last_tasks", "-times", "Show times spent by tasks");
  AddTag("ls_last_tasks", "send_to_all_workers");

  Add("ls_stream_operations_statistics", "debug", "Show statistics about stream operations");
  AddTag("ls_stream_operations_statistics", "send_to_all_workers");

  Add("ls_queue_ranges", "debug", "Show how much information of a queue is at every range");
  AddMandatoryStringArgument("ls_queue_ranges", "name", "Name of the queue we would like to scan");

  Add("data_model_status", "debug", "Show frozen and current data model status");
  Add("data_model_commits", "debug", "Show on-the-fly commits pending to be consolidated");
  Add("data_model_freeze", "debug", "Show frozen and current data model status");
  Add("data_model_cancel_freeze", "debug", "Cancel candidate data model");
  Add("data_model_recover", "debug", "Show frozen and current data model status");

  Add("ls_kv_ranges", "debug", "Show a list of current KVRanges in this SAMSON cluster");

  Add("set_replication_factor", "debug", "Show a list of current KVRanges in this SAMSON cluster");
  AddMandatoryUInt64Argument("set_replication_factor", "factor", "Number of times each block is present in cluster");
  Add("get_replication_factor", "debug", "Show a list of current KVRanges in this SAMSON cluster");


  // ------------------------------------------------------------------
  // MODULES
  // ------------------------------------------------------------------

  Add("ls_modules", "modules", "Show a list of modules installed in SAMSON node workers");
  AddStringArgument("ls_modules", "pattern", "*", "Filter modules with this pattern (* system.* ... )");

  Add("ls_modules_information", "modules", "Show information about loading module process");

  Add("ls_operations", "modules", "Shows a list of available operations");
  AddStringArgument("ls_operations", "pattern", "*", "Filter operations with this pattern (* system.* ... )");
  AddBoolOption("ls_operations", "-v", "Show more details about input/output parameters");

  Add("ls_datas", "modules", "Shows a list of available data-types.");
  AddStringArgument("ls_datas", "pattern", "*", "Filter data-types with this pattern (* system.* ... )");

  Add("ls_modules", "modules", "Show a list of modules installed in SAMSON node workers");
  AddStringArgument("ls_modules", "pattern", "*", "Filter modules with this pattern (* system.* ... )");
  AddTag("ls_modules", "send_to_all_workers");

  Add("ls_operations", "modules", "Shows a list of available operations");
  AddStringArgument("ls_operations", "pattern", "*", "Filter operations with this pattern (* system.* ... )");
  AddBoolOption("ls_operations", "-v", "Show more details about input/output parameters");
  AddTag("ls_operations", "send_to_all_workers");

  Add("ls_datas", "modules", "Shows a list of available data-types.");
  AddStringArgument("ls_datas", "pattern", "*", "Filter data-types with this pattern (* system.* ... )");
  AddTag("ls_datas", "send_to_all_workers");

  Add("ls_local_modules", "modules", "Show a list of modules installed available at this delilah");

  Add("push_module", "modules", "Push a module to the cluster.");
  AddMandatoryStringArgument("push_module", "file", "Local file or directory")->set_options_group("#file");

  Add("clear_modules", "modules", "Remove all previously updated modules");
  AddStringArgument("clear_modules", "pattern", "*", "Block name attern ( see ls_modules_information)");

  // ------------------------------------------------------------------
  // STREAM
  // ------------------------------------------------------------------

  Add("ls_stream_operations", "stream", "Show a list of stream operations",
      "Stream operations are operations that are automatically executed in a SAMSON cluster\n"
      "They consist in applying a particular operation ( defined in a module ) to process data from\n"
      "one or multiple input queues and send result to one or multiple output queues\n"
      "Please, add new stream operations with add_stream_operations command");
  AddBoolOption("ls_stream_operations", "-a",
                "Show hidden stream operations as well ( used internally by the platform )");

  Add("ps_stream_operations", "stream", "Show a list of stream operations with information about execution planning");
  AddStringArgument("ps_stream_operations", "pattern", "*", "Name of the stream operation");
  AddBoolOption("ps_stream_operations", "-state", "Show state of these stream operations");
  AddTag("ps_stream_operations", "send_to_all_workers");

  Add("ps_stream_operations_ranges", "stream", "Show a detailed list of a stream operation (for each range)");
  AddStringArgument("ps_stream_operations_ranges", "pattern", "*", "Name of the stream operation");
  AddBoolOption("ps_stream_operations_ranges", "-state",
                "Show planning state ( paused, error, ready for scheduling...)");
  AddBoolOption("ps_stream_operations_ranges", "-tasks", "Show tasks associated to stream_operations");
  AddBoolOption("ps_stream_operations_ranges", "-properties", "Information about properties of each stream opertion");
  AddBoolOption("ps_stream_operations_ranges", "-data", "Show input and output data processed of each operation");
  AddUInt64Option("ps_stream_operations_ranges", "-w", static_cast<size_t>(-1), "Selected worker");
  AddTag("ps_stream_operations_ranges", "send_to_all_workers");

  Add("add_stream_operation", "stream", "Add a new stream operation to the current SAMSON cluster");
  AddMandatoryStringArgument("add_stream_operation", "name", "Name of the new stream operation");
  AddMandatoryStringArgument("add_stream_operation", "operation", "Name of the operation to run");
  AddBoolOption("add_stream_operation", "-paused", "Start with the operation paused");

  AddStringOption("add_stream_operation", "-input", "", "Input queues to get data from");
  AddStringOption("add_stream_operation", "-output", "", "Output queues to push data to");

  AddBoolOption("add_stream_operation", "-forward", "Forward reduce operations are scheduled. ( No state )");
  AddBoolOption("add_stream_operation", "-update_only", "Only update state for keys with new input values");

  AddStringOption("add_stream_operation", "-env", "", "Environment variables separated by commas");

  Add("remove_stream_operation", "stream", "Remove a previously defined operation with add_stream_operation");
  AddMandatoryStringArgument("remove_stream_operation", "name", "Name of the stream operations to remove");

  Add("remove_all_stream_operations", "stream", "Remove all stream operations");

  Add("wait", "stream", "Wait for all the stream operations activity to be finished");
  Add("wait_batch_tasks", "stream", "Wait for all batch-operation activity to be finished");
  Add("wait_my_batch_tasks", "stream", "Wait for all my batch-operation activity to be finished");

  Add("cancel_stream_operation", "stream", "Cancel of  execution of a particular stream operation");
  AddStringOption("cancel_stream_operation", "name", "", "Name of the stream operation to be canceled");

  Add("set_stream_operation_property", "stream",
      "Set value of an environment property associated to a stream operation (see add_stream_operation)",
      "set_stream_operation_property <stream_operation_name> <variable_name> <value>");
  AddMandatoryStringArgument("set_stream_operation_property", "operation_name",
                             "Name of the affected stream operation");
  AddMandatoryStringArgument("set_stream_operation_property", "variable_name", "Name of the variable");
  AddMandatoryStringArgument("set_stream_operation_property", "value", "Value of the variable");

  Add("unset_stream_operation_property", "stream",
      "Remove an environment property associated to a stream operation (see add_stream_operation)",
      "unset_stream_operation_property <stream_operation_name> <variable_name>");

  AddMandatoryStringArgument("unset_stream_operation_property", "operation_name",
                             "Name of the affected stream operation");
  AddMandatoryStringArgument("unset_stream_operation_property", "variable_name", "Name of the variable");

  Add("add_queue_connection", "stream", "Connect a queue to other queues to duplicate data automatically");

  AddMandatoryStringArgument("add_queue_connection", "source_queue", "Source queue");
  AddMandatoryStringArgument("add_queue_connection", "target_queue", "Target queue ( or queues )");

  Add("rm_queue_connection", "stream", "Remove a connection between queues (see  add_queue_connection )");
  AddMandatoryStringArgument("rm_queue_connection", "source_queue", "Source queue");
  AddMandatoryStringArgument("rm_queue_connection", "target_queue", "Target queue ( or queues )");

  Add("ls_queue_connections", "stream", "Show a list with all queue connections ( see add_queue_connection ) ");
  AddBoolOption("ls_queue_connections", "-a", "Show hidden items as well ( used internally by the platform )");

  Add("ps_tasks", "stream", "Get a list of current stream tasks currently running in all workers");
  AddUInt64Option("ps_tasks", "-w", static_cast<size_t>(-1), "Specify worker to request list of tasks");
  AddBoolOption("ps_tasks", "-data", "Get detailed information of input/output data");
  AddBoolOption("ps_tasks", "-blocks", "Get detailed information of input/output blocks");
  AddTag("ps_tasks", "send_to_all_workers");

  Add("ls_workers", "stream", "Get a list of current workers");
  AddBoolOption("ls_workers", "-engine", "Show details about underlying engines at worker nodes");
  AddBoolOption("ls_workers", "-disk", "Show details about underlying engines at worker nodes");
  AddTag("ls_workers", "send_to_all_workers");

  Add("init_stream", "stream", "Execute am initialization script to setup some automatic stream operations",
      "init_stream [-prefix pref] <script_name>\n"
      "          [-prefix pref_string]         It is used to name operations and queues\n"
      "          <script_name>    Name of the script (e.g. module.script)\n");
  AddStringOption("init_stream", "-prefix", "", "Prefix to name operations and queues");
  AddMandatoryStringArgument("init_stream", "operation", "Name of the stream script to be installed");

  Add("ps_workers", "stream", "Show a list of commands being executed in each node of the cluster",
      "ps_workers <command pattern>\n");

  Add("defrag", "stream", "Defrag content of a particular queue", "defrag <queue> <destination_queue>\n");

  // BATCH

  Add("run", "batch", "Run a particular operation over queues in batch mode");
  AddMandatoryStringArgument("run", "operation", "Name of the operation to run");
  AddStringOption("run", "-input", "", "Input queues to get data from");
  AddStringOption("run", "-output", "", "Output queues to push data to");

  AddStringOption("run", "-env", "", "Environment variables separated by commas");

  Add("ls_batch_operations", "batch", "List scheduled batch operations");
  AddBoolOption("ls_batch_operations", "-input", "Show real input pending to be processed");
  AddBoolOption("ls_batch_operations", "-output", "Show output produced by batch operations");

  Add("clear_batch_operations", "batch", "Clear finished batch operations");
  AddBoolOption("clear_batch_operations", "-a", "Clear also unfinished tasks");

  // ------------------------------------------------------------------
  // PUSH&POP
  // ------------------------------------------------------------------

  Add("push", "push&pop", "Push content of a local file or a directory to a set of queues");
  AddMandatoryStringArgument("push", "file", "Local file or directory")->set_options_group("#file");
  AddMandatoryStringArgument("push", "queue", "queue or queues to push data (queue1 queue2 queue3 ...)");

  Add("pop", "push&pop", "Pop content of a queue to a local directory");
  AddMandatoryStringArgument("pop", "queue", "Name of the queue we want to pop");
  AddMandatoryStringArgument("pop", "file_name", "Local directory to save downloaded blocks");
  AddBoolOption("pop", "-force", "Delete local directory if it exists");
  AddBoolOption("pop", "-show", "Show first lines of the content once downloaded");

  Add("ls_local_push_operations", "push&pop", "Show list of pending push items");

  Add("connect_to_queue", "push&pop", "Connect to a queue to receive live data from a SAMSON cluster");
  AddMandatoryStringArgument("connect_to_queue", "queue", "Source queue ( see ls command )");

  Add("disconnect_from_queue", "push&pop",
      "Disconnects from a particular queue to not receive live data from a SAMSON cluster");
  AddMandatoryStringArgument("disconnect_from_queue", "queue", "Source queue ( see ls command )");

  Add("ls_pop_connections", "push&pop", "Show a list of connections to receive live data from SAMSON.",
      "ls_pop_connections");

  Add("show_local_queue", "push&pop", "Show contents of a local queue downloaded using pop",
      "Note: Modules should be installed locally");

  AddMandatoryStringArgument("show_local_queue", "queue", "Local queue to be displayed");
  AddBoolOption("show_local_queue", "-header", "Display only header information");
  AddBoolOption("show_local_queue", "-show_hg", "Show hash-group for each key-value");
  AddUInt64Option("show_local_queue", "-limit", 0, "Limit the number of records displayed");

  Add("push_queue", "push&pop", "Push content of a queue to another queue/s");
  AddMandatoryStringArgument("push_queue", "queue_from", "Source queue to get data from");
  AddMandatoryStringArgument("push_queue", "queue_to", "Target queue to push data to");

  // ------------------------------------------------------------------
  // LOG
  // ------------------------------------------------------------------


  Add("wlog_show", "log", "Show logs received from workers (via logServer)");
  AddStringArgument("wlog_show", "host", "localhost", "logServer hostname ( optional :port)");
  AddStringOption("wlog_show", "format", LOG_DEFAULT_FORMAT_CONSOLE, "Format of the logs");
  AddStringOption("wlog_show", "filter", "", "Filter of logs to show");
  Add("wlog_hide", "debug", "Not show logs received from workers (via logServer) anymore");

  Add("wlog_status", "log", "Show current status of the logs from workers");
  AddTag("wlog_status", "send_to_all_workers");

  Add("wlog_all_channels", "log", "Show all possible log channels to be activated");

  Add("wlog_set_log_server", "log", "Set the host for log server in all workers");
  AddMandatoryStringArgument("wlog_set_log_server", "host", "Log server host");
  AddTag("wlog_set_log_server", "send_to_all_workers");

  Add("wlog_set", "log", "Set the lovel of logs for a particular channel");
  AddMandatoryStringArgument("wlog_set", "channel_pattern", "Channel ( or channel pattern )");
  AddStringArgument("wlog_set", "log_level", "*", "Level of logs");
  AddTag("wlog_set", "send_to_all_workers");


  Add("log_help", "log", "Show help about log_X commands to interact with the log system");

  Add("log_to_screen", "log", "Add a log plugin to emit logs to screen");
  AddStringOption("log_to_screen", "-name", "screen", "Name of the plugin");
  AddStringOption("log_to_screen", "-format", LOG_DEFAULT_FORMAT, "Format of logs on screen");

  Add("log_to_file", "log", "Add a log plugin to emit logs to a file");
  AddMandatoryStringArgument("log_to_file", "filename", "File to be created to store logs");
  AddStringOption("log_to_file", "name", "file", "Name of the plugin");

  Add("log_to_server", "log", "Add a log plugin to emit logs to a server");
  AddMandatoryStringArgument("log_to_server", "host", "Host of the log server");
  AddMandatoryStringArgument("log_to_server", "filename",
                             "File to be created to store logs when connection is not available");
  AddStringOption("log_to_server", "name", "server", "Name of the plugin");

  // Show information
  Add("log_show_fields", "log", "Show available fields for logs");
  Add("log_show_plugins", "log", "Show current plugins for logs");
  Add("log_show_channels", "log", "Show logs generatd for all channels and all plugins");
  AddBoolOption("log_show_channels", "rates", "Show information about logs generated in bytes/second");
  AddBoolOption("log_show_channels", "v", "Show more information about generated logs");

  // Set level of log messages for a particular channel
  Add("log_set", "log",
      "Set log-level to specified value for some log-channels ( and for some log-plugins if specified )");
  AddMandatoryStringArgument("log_set", "channel_pattern", "Name (or pattern) of log channel");
  AddMandatoryStringArgument("log_set", "log_level", "Level of log D,M,V,V2,V3,V4,V5,W,E,X ( type - for no log )");
  AddStringArgument("log_set", "plugin_pattern", "*", "Name (or pattern) of log-plugin (type * or nothing for all)");

  // Set level of log messages for a particular channel
  Add("log_add", "log",
      "Set log-level at least to specified value for some log-channels ( and for some log-plugins if specified )");
  AddMandatoryStringArgument("log_add", "channel_pattern", "Name (or pattern) of log channel");
  AddMandatoryStringArgument("log_add", "log_level", "Level of log D,M,V,V2,V3,V4,V5,W,E,X ( type - for no log )");
  AddStringArgument("log_add", "plugin_pattern", "*", "Name (or pattern) of log-plugin (type * or nothing for all)");

  // Set level of log messages for a particular channel
  Add("log_remove", "log", "Unset logs for some log-channels ( and for some log-plugins if specified )");
  AddMandatoryStringArgument("log_remove", "channel_pattern", "Name (or pattern) of log channel");
  AddStringArgument("log_remove", "plugin_pattern", "*",
                    "Name (or pattern) of log-plugin (type * or nothing for all)");
}
}
