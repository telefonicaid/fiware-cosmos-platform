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


#include "samson/delilah/DelilahCommandCatalogue.h"  // Own interface

#include "samson/common/common.h"


namespace samson {
DelilahCommandCatalogue::DelilahCommandCatalogue() {
  // ------------------------------------------------------------------
  // SETUP
  // ------------------------------------------------------------------

  AddCommand("show_cluster_setup", "setup", "Show cluster setup", "Display cluster configuration");
  AddCommand("show_cluster_assignation", "setup", "Show cluster assignment",
             "Display the hash-groups assigned to each worker");
  AddCommand("ls_local_connections", "delilah", "Display the connection status for this delilah session");

  // ------------------------------------------------------------------
  // DELILAH
  // ------------------------------------------------------------------

  AddCommand("connect", "delilah", "Connect to the specified SAMSON cluster");
  AddMandatoryStringArgument("connect", "host", "SAMSON node(s) to connect to (\"host:port host:port..\")");
  AddStringOption("connect", "-digest", "", "Username and password used to connect to the SAMSON cluster");

  AddCommand("disconnect", "delilah", "Disconnect from a SAMSON cluster");

  AddCommand("alerts", "delilah", "Enable/disable console alerts");
  AddStringOptionsArgument("alerts", "action", "Options:on:off", "Enable/disable console alerts");

  AddCommand("show_alerts", "delilah", "Show previously sent alerts for the current delilah session");

  AddCommand("open_alerts_file", "delilah", "Open a local file to store all received alerts");
  AddMandatoryStringArgument("open_alerts_file", "file", "Local file to store alerts");

  AddCommand("close_alerts_file", "delilah", "Stop saving alerts to a local file");

  AddCommand("verbose", "delilah", "Enable/disable verbose reporting mode");
  AddStringOptionsArgument("verbose", "action", "Options::on:off");

  AddCommand("send_alert", "delilah", "Send an alert to all active delilah sessions");
  AddStringOption("send_alert", "-worker", "", "Select broadcaster worker");
  AddBoolOption("send_alert", "-error", "Mark message as an error");
  AddBoolOption("send_alert", "-warning", "Mark message as a warning");
  AddStringArgument("send_alert", "message", "", "Content of the message");

  AddCommand("ps", "delilah", "Show information about delilah processes");
  AddBoolOption("ps", "-clear", "Remove finished processes");
  AddStringArgument("ps", "id", "", "Show more information for this process");

  AddCommand("set", "delilah", "Set an environment variable");
  AddStringArgument("set", "name", "", "Variable name");
  AddStringArgument("set", "value", "", "Variable value");

  AddCommand("unset", "delilah", "Unset a previously defined environment variable");
  AddStringArgument("set", "name", "", "Variable name");

  AddCommand("ls_local", "local", "Show a directory listing combined with local queue information");
  AddStringArgument("ls_local", "file", "*", "File or pattern to be displayed");

  AddCommand("ls_local_queues", "local", "Show a list of local queues");
  AddStringArgument("ls_local_queues", "file", "*", "File or pattern to be displayed");

  AddCommand("rm_local", "local", "Recursively remove a file or directory");
  AddMandatoryStringArgument("rm_local", "file", "File or directory to be removed");

  AddCommand("history", "local", "Show the command history");
  AddIntOption("history", "limit", 0);

  AddCommand("quit", "delilah", "Quit delilah console");

  AddCommand("reload_modules_local", "delilah", "Reload local modules");

  // ------------------------------------------------------------------
  // DATA MANIPULATION
  // ------------------------------------------------------------------

  AddCommand("ls", "data", "Show a list of all data queues in the SAMSON cluster");
  AddStringArgument("ls", "pattern", "*", "Pattern of the queues to show");
  AddBoolOption("ls", "-a", "Include internal queues");
  AddBoolOption("ls", "-commit", "Add commit id information for each queue");
  AddBoolOption("ls", "-blocks", "Add #blocks information for each queue");

  AddCommand("add", "data", "Create a new queue");
  AddMandatoryStringArgument("add", "queue", "Name of the new queue");
  AddMandatoryStringArgument("add", "key_format", "Format for the key's in new queue (txt is accepted)");
  AddStringArgument("add", "value_format", "txt", "Format for the value's in new queue (txt is accepted)");

  AddCommand("remove_all_queues", "data", "Remove all queues in the SAMSON cluter");

  AddCommand("remove_all", "data", "Remove all queues and stream_operations in the SAMSON cluster");

  AddCommand("reset_samson", "data", "Remove all queues, stream_operations & modules in the SAMSON cluster");

  AddCommand("rm", "data", "Remove a queue");
  AddMandatoryStringArgument("rm", "queue", "Queue to be removed");

  AddCommand("set_queue_property", "data", "Specify the value of a property in a queue ");
  AddMandatoryStringArgument("set_queue_property", "queue", "Queue to be updated");
  AddMandatoryStringArgument("set_queue_property", "property", "Name of the property we want to update");
  AddMandatoryStringArgument("set_queue_property", "value", "Value for the selected property");

  AddCommand("unset_queue_property", "data", "Remove a previously defined property for a queue");
  AddMandatoryStringArgument("unset_queue_property", "queue", "Queue to be updated");
  AddMandatoryStringArgument("unset_queue_property", "property", "Name of the property we want to update");

  // ------------------------------------------------------------------
  // Debug
  // ------------------------------------------------------------------

  AddCommand("ls_connections", "debug", "(Debug) Show status of all connections in the cluster");
  AddUInt64Option("ls_connections", "-w", SIZE_T_UNDEFINED, "Specify the worker used to request the connections list");
  AddTag("ls_connections", "send_to_all_workers");

  AddCommand("ls_network_queues", "debug", "(Debug) Show status of all connections in the cluster");
  AddUInt64Option("ls_network_queues", "-w", SIZE_T_UNDEFINED, "Specify the worker used to request this list");
  AddTag("ls_network_queues", "send_to_all_workers");

  AddCommand("threads", "debug", "(Debug) Show current threads in this delilah");

  AddCommand("ls_blocks", "debug", "(Debug) Show a list of data blocks managed by each SAMSON node");
  AddStringArgument("ls_blocks", "pattern", "*", "Pattern to find specific blocks");
  AddUInt64Option("ls_blocks", "-w", SIZE_T_UNDEFINED, "Specify the worker used to request this list");
  AddTag("ls_blocks", "send_to_all_workers");
  AddBoolOption("ls_blocks", "-info", "Show more information about priority of this block");

  AddCommand("ls_queue_blocks", "debug", "(Debug) Show a detailed list of the blocks included in every queue");
  AddStringArgument("ls_queue_blocks", "pattern", "*", "Pattern to find specific queues");
  AddBoolOption("ls_queue_blocks", "-a", "Include internal queues");

  AddCommand("ls_buffers", "debug", "(Debug) Show the list of data buffers managed in a SAMSON cluster.");
  AddUInt64Option("ls_buffers", "-w", SIZE_T_UNDEFINED, "Specify the worker used to request this list");
  AddTag("ls_buffers", "send_to_all_workers");

  AddCommand("ls_block_requests", "debug", "(Debug) Show current block requests in all SAMSON nodes");
  AddStringArgument("ls_block_requests", "pattern", "*", "Select pattern for blocks");
  AddTag("ls_block_requests", "send_to_all_workers");

  AddCommand("ls_block_defrags", "debug", "(Debug) Show block de-fragmentation operations in all SAMSON nodes");
  AddTag("ls_block_defrags", "send_to_all_workers");

  AddCommand("ls_last_commits", "debug", "(Debug) Show recently committed data models");

  AddCommand("ls_last_commits_debug", "debug", "(Debug) Show recent commit tests for the data model");
  AddTag("ls_last_commits_debug", "send_to_all_workers");

  AddCommand("ls_last_tasks", "debug", "(Debug) Show the last 100 tasks scheduled for execution against all nodes");
  AddBoolOption("ls_last_tasks", "-times", "Include the execution time for each task");
  AddTag("ls_last_tasks", "send_to_all_workers");

  AddCommand("ls_queue_ranges", "debug", "(Debug) Display the range of data for a given queue");
  AddMandatoryStringArgument("ls_queue_ranges", "name", "Name of the queue we would like to scan");

  AddCommand("data_model_status", "debug", "(Debug) Show the status of frozen and current data models");
  AddCommand("data_model_commits", "debug", "(Debug) Show on-the-fly commits pending to be consolidated");
  AddCommand("data_model_freeze", "debug", "(Debug) Freeze the current data model");
  AddCommand("data_model_cancel_freeze", "debug", "(Debug) Cancel candidate data model");
  AddCommand("data_model_recover", "debug", "(Debug) Recover the previously frozen data model");

  AddCommand("ls_kv_ranges", "debug", "(Debug) Show a list of key-value ranges in this SAMSON cluster");

  AddCommand("set_replication_factor", "debug", "(Experimental) Set replication factor in the cluster");
  AddMandatoryUInt64Argument("set_replication_factor", "factor", "Number of times each block is present in cluster");
  AddCommand("get_replication_factor", "debug", "Show a list of key-value ranges in this SAMSON cluster");

  // ------------------------------------------------------------------
  // MODULES
  // ------------------------------------------------------------------

  AddCommand("ls_modules", "modules", "Show a list of modules installed at each SAMSON node");
  AddStringArgument("ls_modules", "pattern", "*", "Filter modules with this pattern (* system.* ... )");
  AddTag("ls_modules", "send_to_all_workers");

  AddCommand("ls_modules_information", "modules", "Show the results of previously executed push_module command");

  AddCommand("ls_operations", "modules", "Shows a list of available operations");
  AddStringArgument("ls_operations", "pattern", "*", "Filter operations with this pattern (* system.* ... )");
  AddBoolOption("ls_operations", "-v", "Show more details about input/output parameters");

  AddCommand("ls_datas", "modules", "Show a list of the available data-types.");
  AddStringArgument("ls_datas", "pattern", "*", "Filter data-types with this pattern (* system.* ... )");


  AddCommand("ls_operations", "modules", "Shows a list of available operations");
  AddStringArgument("ls_operations", "pattern", "*", "Filter operations with this pattern (* system.* ... )");
  AddBoolOption("ls_operations", "-v", "Show more details about input/output parameters");
  AddTag("ls_operations", "send_to_all_workers");

  AddCommand("ls_datas", "modules", "Shows a list of available data-types.");
  AddStringArgument("ls_datas", "pattern", "*", "Filter data-types with this pattern (* system.* ... )");
  AddTag("ls_datas", "send_to_all_workers");

  AddCommand("ls_local_modules", "modules", "Show a list of locally installed modules");

  AddCommand("push_module", "modules", "Upload a module to the cluster");
  AddMandatoryStringArgument("push_module", "file", "Local file or directory")->set_options_group("#file");

  AddCommand("push_internal_modules", "modules", "Upload internal modules to current cluster");

  AddCommand("clear_modules", "modules", "Remove all previously uploaded modules");
  AddStringArgument("clear_modules", "pattern", "*", "Block name attern ( see ls_modules_information)");

  // ------------------------------------------------------------------
  // STREAM
  // ------------------------------------------------------------------

  AddCommand("ls_stream_operations", "stream", "Show a list of stream operations",
             "Stream operations are operations that are automatically executed in a SAMSON cluster\n"
             "They consist in applying a particular operation ( defined in a module ) to process data from\n"
             "one or multiple input queues and send result to one or multiple output queues\n"
             "Please, add new stream operations with add_stream_operations command");
  AddBoolOption("ls_stream_operations", "-a", "Include internal stream operations");
  AddBoolOption("ls_stream_operations", "-id", "Include numerical identifier of each operation");

  AddCommand("ls_stream_operations_statistics", "stream", "Show stream operation statistics for the cluster");
  AddTag("ls_stream_operations_statistics", "send_to_all_workers");

  AddCommand("ps_stream_operations", "stream",
             "Show a list of stream operations with information about execution planning");
  AddStringArgument("ps_stream_operations", "pattern", "*", "Name of the stream operation");
  AddBoolOption("ps_stream_operations", "-state", "Show state of these stream operations");
  AddTag("ps_stream_operations", "send_to_all_workers");

  AddCommand("ps_stream_operations_ranges", "stream", "Show the range data for the specified stream operation");
  AddStringArgument("ps_stream_operations_ranges", "pattern", "*", "Name of the stream operation");
  AddBoolOption("ps_stream_operations_ranges", "-state",
                "Show planning state ( paused, error, ready for scheduling...)");
  AddBoolOption("ps_stream_operations_ranges", "-tasks", "Show tasks associated to stream_operations");
  AddBoolOption("ps_stream_operations_ranges", "-properties", "Show stream operation properites");
  AddBoolOption("ps_stream_operations_ranges", "-data", "Show detail regarding input/output data");
  AddUInt64Option("ps_stream_operations_ranges", "-w", SIZE_T_UNDEFINED, "Specify the worker used to request this list");
  AddTag("ps_stream_operations_ranges", "send_to_all_workers");

  AddCommand("add_stream_operation", "stream", "Add a new stream operation to the SAMSON cluster");
  AddMandatoryStringArgument("add_stream_operation", "name", "Stream operation alias");
  AddMandatoryStringArgument("add_stream_operation", "operation", "Name of the operation to run");
  AddBoolOption("add_stream_operation", "-paused", "Start with the operation paused");

  AddStringOption("add_stream_operation", "-input", "", "Input queues to read data");
  AddStringOption("add_stream_operation", "-output", "", "Output queues to write data");

  AddBoolOption("add_stream_operation", "-forward", "Mark as a forward reduce operation (no state is maintained)");
  AddBoolOption("add_stream_operation", "-update_only", "Only update state for keys with new input values");

  AddStringOption("add_stream_operation", "-env", "", "List of environment variables (key=value) separated by commas");

  AddCommand("remove_stream_operation", "stream", "Remove a previously scheduled streaming operation");
  AddMandatoryStringArgument("remove_stream_operation", "name", "Alias of the stream operation to be removed");

  AddCommand("remove_all_stream_operations", "stream", "Remove all stream operations");

  AddCommand("wait", "stream", "Wait for all input queues used in stream operations to empty");
  AddCommand("wait_batch_tasks", "stream", "Wait for all batch operation tasks to complete");
  AddCommand("wait_my_batch_tasks", "stream", "Wait for my batch operations to complete");

  AddCommand("set_stream_operation_property", "stream",
             "Set value of an environment property associated to a stream operation (see add_stream_operation)",
             "set_stream_operation_property <stream_operation_name> <variable_name> <value>");
  AddMandatoryStringArgument("set_stream_operation_property", "operation_name", "Stream operation alias");
  AddMandatoryStringArgument("set_stream_operation_property", "variable_name", "Variable name");
  AddMandatoryStringArgument("set_stream_operation_property", "value", "Variable value");

  AddCommand("unset_stream_operation_property", "stream", "Unset environment variables for a stream operation");
  AddMandatoryStringArgument("unset_stream_operation_property", "operation_name", "Stream operation alias");
  AddMandatoryStringArgument("unset_stream_operation_property", "variable_name", "Variable name");

  AddCommand("add_queue_connection", "stream",
             "Continually replicate the contents of an existing queue with one or more new queues");

  AddMandatoryStringArgument("add_queue_connection", "source_queue", "Source queue");
  AddMandatoryStringArgument("add_queue_connection", "target_queue", "Target queue ( or queues )");

  AddCommand("rm_queue_connection", "stream", "Remove a connection between queues (see add_queue_connection )");
  AddMandatoryStringArgument("rm_queue_connection", "source_queue", "Source queue");
  AddMandatoryStringArgument("rm_queue_connection", "target_queue", "Target queue ( or queues )");

  AddCommand("ls_queue_connections", "stream", "Show a list of linked queues");
  AddBoolOption("ls_queue_connections", "-a", "Include internal queues");

  AddCommand("ps_tasks", "stream", "Get a list of currently running stream tasks in all workers");
  AddUInt64Option("ps_tasks", "-w", SIZE_T_UNDEFINED, "Specify the worker used to request the task list");
  AddBoolOption("ps_tasks", "-data", "Show detail regarding input/output data");
  AddBoolOption("ps_tasks", "-blocks", "Show detail regarding input/output blocks");
  AddTag("ps_tasks", "send_to_all_workers");

  AddCommand("ls_workers", "stream", "Get a list of worker nodes");
  AddBoolOption("ls_workers", "-modules", "Show if modules are available at each worker");
  AddBoolOption("ls_workers", "-traffic", "Show information about data rate to/from disk, network & Zookeeper");
  AddBoolOption("ls_workers", "-data_model", "Show information about data-model version at each worker");
  AddBoolOption("ls_workers", "-engine", "For each worker node, detail the state of the underlying engine");
  AddBoolOption("ls_workers", "-disk", "For each worker node, detail the state of the underlying disk manager");

  AddTag("ls_workers", "send_to_all_workers");

  AddCommand("init_stream", "stream", "Execute an script to initialize a set of automatic stream operations",
             "init_stream [-prefix pref] <script_name>\n"
             "          [-prefix pref_string]         It is used to name operations and queues\n"
             "          <script_name>    Name of the script (e.g. module.script)\n");
  AddStringOption("init_stream", "-prefix", "", "Prefix to name operations and queues");
  AddMandatoryStringArgument("init_stream", "operation", "Name of the stream script to be executed");

  AddCommand("ps_workers", "stream", "Show a list of commands currently being executed in each node");

  // ------------------------------------------------------------------
  // BATCH
  // ------------------------------------------------------------------

  AddCommand("run", "batch", "Run the specified operation in batch mode");
  AddMandatoryStringArgument("run", "operation", "Operation name to execute");
  AddStringOption("run", "-input", "", "Input queues to read data");
  AddStringOption("run", "-output", "", "Output queues to write data");

  AddStringOption("run", "-env", "", "List of environment variables (key=value) separated by commas");

  AddCommand("ls_batch_operations", "batch", "List scheduled batch operations");
  AddBoolOption("ls_batch_operations", "-input", "Show input pending processing");
  AddBoolOption("ls_batch_operations", "-output", "Show the output summary");

  AddCommand("clear_batch_operations", "batch", "Clear finished batch operations");
  AddBoolOption("clear_batch_operations", "-a", "Clear also unfinished tasks");

  // ------------------------------------------------------------------
  // PUSH&POP
  // ------------------------------------------------------------------

  AddCommand("push", "push&pop", "Push/upload the contents of a local file or directory to one or more queues");
  AddMandatoryStringArgument("push", "file", "Local file or directory")->set_options_group("#file");
  AddMandatoryStringArgument("push", "queue", "Queue or queues to push data to (queue1 queue2 queue3 ...)");

  AddCommand("pop", "push&pop", "Pop the contents of a queue to the local filesystem");
  AddMandatoryStringArgument("pop", "queue", "Queue to be popped");
  AddMandatoryStringArgument("pop", "file_name", "The name of a folder where the data is downloaded to");
  AddBoolOption("pop", "-force", "Delete local directory if it exists");
  AddBoolOption("pop", "-show", "Show first key-values or lines (max 100) of the content once downloaded");

  AddCommand("ls_local_push_operations", "push&pop", "Show a list of items awaiting upload");

  AddCommand("connect_to_queue", "push&pop", "Connect to a queue to receive live data from a SAMSON cluster");
  AddMandatoryStringArgument("connect_to_queue", "queue", "Source queue ( see ls command )");

  AddCommand("disconnect_from_queue", "push&pop",
             "Disconnects from a particular queue to not receive live data from a SAMSON cluster");
  AddMandatoryStringArgument("disconnect_from_queue", "queue", "Source queue ( see ls command )");

  AddCommand("ls_pop_connections", "push&pop", "Show a list of connections to receive live data from SAMSON.",
             "ls_pop_connections");

  AddCommand("show_local_queue", "push&pop", "Show contents of a local queue downloaded using pop",
             "Note: Modules should be installed locally");

  AddMandatoryStringArgument("show_local_queue", "queue", "Local queue to be displayed");
  AddBoolOption("show_local_queue", "-header", "Display only header information");
  AddBoolOption("show_local_queue", "-show_hg", "Show hash-group for each key-value");
  AddUInt64Option("show_local_queue", "-limit", 0, "Limit the number of records displayed");

  AddCommand("push_queue", "push&pop", "Push content of a queue to another queue/s");
  AddMandatoryStringArgument("push_queue", "queue_from", "Source queue to get data from");
  AddMandatoryStringArgument("push_queue", "queue_to", "Target queue to push data to");

  // ------------------------------------------------------------------
  // LOG
  // ------------------------------------------------------------------


  AddCommand("wlog_show", "log", "Show logs received from workers (via logServer)");
  AddStringArgument("wlog_show", "host", "localhost", "logServer hostname ( optional :port)");
  AddStringOption("wlog_show", "format", LOG_DEFAULT_FORMAT_CONSOLE, "Format of the logs");
  AddStringOption("wlog_show", "filter", "", "Filter of logs to show");
  AddCommand("wlog_hide", "debug", "Not show logs received from workers (via logServer) anymore");

  AddCommand("wlog_status", "log", "Show status of the logs from workers");
  AddTag("wlog_status", "send_to_all_workers");

  AddCommand("wlog_all_channels", "log", "Show all possible log channels to be activated");

  AddCommand("wlog_set_log_server", "log", "Set the log server to be used by all workers");
  AddMandatoryStringArgument("wlog_set_log_server", "host", "Log server host");
  AddTag("wlog_set_log_server", "send_to_all_workers");

  AddCommand("wlog_set", "log", "For a given channel set the log level");
  AddMandatoryStringArgument("wlog_set", "channel_pattern", "Channel ( or channel pattern )");
  AddStringArgument("wlog_set", "log_level", "*", "Log level D,M,V,W,E,X ( type '-' for no log )");
  AddTag("wlog_set", "send_to_all_workers");


  AddCommand("log_help", "log", "Show the log system help");

  AddCommand("log_to_screen", "log", "Add a log plugin to send logs to screen");
  AddStringOption("log_to_screen", "-name", "screen", "Name of the plugin");
  AddStringOption("log_to_screen", "-format", LOG_DEFAULT_FORMAT, "Log format");

  AddCommand("log_to_file", "log", "Add a log plugin to send logs to a file");
  AddMandatoryStringArgument("log_to_file", "filename", "File to be used to store logs");
  AddStringOption("log_to_file", "name", "file", "Name of the plugin");

  AddCommand("log_to_server", "log", "Add a log plugin to send logs to a server");
  AddMandatoryStringArgument("log_to_server", "host", "Host of the log server");
  AddMandatoryStringArgument("log_to_server", "filename",
                             "File to be created to store logs when connection is not available");
  AddStringOption("log_to_server", "name", "server", "Name of the plugin");

  // Show information
  AddCommand("log_show_fields", "log", "Show available log fields");
  AddCommand("log_show_plugins", "log", "Show current log plugins");
  AddCommand("log_show_channels", "log", "Show logging information for channels and plugins");
  AddBoolOption("log_show_channels", "rates", "Show log data rates (bytes/sec)");
  AddBoolOption("log_show_channels", "v", "Show more information about generated logs");

  // Set level of log messages for a particular channel
  AddCommand("log_set", "log",
             "Set log-level to specified value for some log-channels ( and for some log-plugins if specified )");
  AddMandatoryStringArgument("log_set", "channel_pattern", "Name (or pattern) of log channel");
  AddMandatoryStringArgument("log_set", "log_level", "Log level: D,M,V,W,E,X ( type '-' for no log )");
  AddStringArgument("log_set", "plugin_pattern", "*", "Name (or pattern) of log-plugin (type * or nothing for all)");

  // Set level of log messages for a particular channel
  AddCommand(
    "log_add", "log",
    "Set log-level at least to specified value for some log-channels ( and for some log-plugins if specified )");
  AddMandatoryStringArgument("log_add", "channel_pattern", "Name (or pattern) of log channel");
  AddMandatoryStringArgument("log_add", "log_level", "Log level D,M,V,W,E,X ( type '-' for no log )");
  AddStringArgument("log_add", "plugin_pattern", "*", "Name (or pattern) of log-plugin (type * or nothing for all)");

  // Set level of log messages for a particular channel
  AddCommand("log_remove", "log", "Unset logs for some log-channels ( and for some log-plugins if specified )");
  AddMandatoryStringArgument("log_remove", "channel_pattern", "Name (or pattern) of log channel");
  AddStringArgument("log_remove", "plugin_pattern", "*",
                    "Name (or pattern) of log-plugin (type * or nothing for all)");
}
}
