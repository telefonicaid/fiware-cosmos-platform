

#include "au/S.h"
#include "au/Singleton.h"
#include "au/ThreadManager.h"
#include "au/network/RESTServiceCommand.h"
#include "au/string.h"
#include "parseArgs/paConfig.h"

#include "Channel.h"
#include "ChannelAdaptor.h"
#include "StreamConnector.h"  // Own interface
#include "StreamConnectorService.h"

extern char input_splitter_name[1024];
extern bool interactive;


namespace stream_connector {
void *review_samson_connector(void *p) {
  StreamConnector *samson_connector = (StreamConnector *)p;

  // Endless loop waiting for data....
  while (true) {
    samson_connector->review();
    usleep(100000);
  }
  return NULL;
}

StreamConnector::StreamConnector() : token("SamsonConnector::token_channels") {
  // No service by default
  service = NULL;

  // No interchannel service by default
  inter_channel_listener = NULL;

  // Create a thread to review
  pthread_t t;
  au::ThreadManager::shared()->addThread("SamsonConector:review", &t, NULL, review_samson_connector, this);
}

void StreamConnector::init_remove_connections_service() {
  if (service) {
    return;   // Already created
  }
  service = new StreamConnectorService(this);
  au::Status s = service->InitService();
  if (s != au::OK) {
    log(new Log("SamsonConnector"
                , "Warning"
                , "Not possible to start service to receive connections from samsonConnectorClient"));
    delete service;
    service = NULL;
  } else {
    log(new Log("SamsonConnector"
                , "Message"
                , "Console Service started"));
  }
}

void StreamConnector::init_reset_service() {
  if (rest_service) {
    return;   // Already init
  }
  // Create the rest service
  rest_service = new au::network::RESTService(sc_web_port, this);
  au::Status s = rest_service->InitService();
  if (s != au::OK) {
    log(new Log("REST", "error", au::str("Error opening REST interface on port %d", sc_web_port)));
    delete rest_service;
    rest_service = NULL;
  }
}

void StreamConnector::init_inter_channel_connections_service() {
  if (inter_channel_listener)
    return;

  // Create a listener over port SAMSON_CONNECTOR_INTERCHANNEL_PORT to receive inter channel connections
  int p = SAMSON_CONNECTOR_INTERCHANNEL_PORT;
  inter_channel_listener = new au::NetworkListener(this);
  au::Status s = inter_channel_listener->InitNetworkListener(p);

  if (s != au::OK) {
    log(new Log("SamsonConnector"
                , "Warning"
                , au::str("Not possible to init inter-channel listener at port %d. This could be a major issue", p)));

    // Delete instance
    delete inter_channel_listener;
    inter_channel_listener = NULL;
  } else {
    log(new Log("SamsonConnector"
                , "Message"
                , "Interchannel service started"));
  }
}

// au::network::RESTServiceInterface
void StreamConnector::process(au::SharedPointer<au::network::RESTServiceCommand> command) {
  // Mutex protection for whatever update over stream connector
  au::TokenTaker tt(&token);

  if (command->path_components().size() == 0) {
    // No command error
    command->AppendFormatedError("No command provided");
    return;
  }

  if (command->path_components()[0] == "summary") {
    au::tables::Table *table = getSummaryTable();
    command->Append(table->strFormatted(command->format()));
    delete table;
    return;
  }

  if (command->path_components()[0] == "logs") {
    LogManager *log_manager = au::Singleton<LogManager>::shared();
    au::tables::Table *table = log_manager->getLogsTable(100);
    command->Append(table->strFormatted(command->format()));
    delete table;
    return;
  }

  if (command->path_components()[0] == "channels") {
    if (command->path_components().size() == 1) {
      au::tables::Table *table = getChannelsTable();
      command->Append(table->strFormatted(command->format()));
      delete table;
      return;
    } else if (command->path_components().size() == 2) {
      au::tables::Table *table = getChannelsTable(command->path_components()[1]);
      command->Append(table->strFormatted(command->format()));
      delete table;
      return;
    } else {
      command->AppendFormatedError("Wrong format. Only /channels/format is supported");
      return;
    }

    return;
  }


  if (command->path_components()[0] == "adaptors") {
    if (command->path_components().size() == 1) {
      au::tables::Table *table = getItemsTable();
      command->Append(table->strFormatted(command->format()));
      return;
    } else if (command->path_components().size() == 2) {
      au::tables::Table *table = getItemsTable(command->path_components()[1]);
      command->Append(table->strFormatted(command->format()));
      return;
    } else {
      command->AppendFormatedError("Wrong format. Only /adaptors/format is supported");
      return;
    }
  }

  if (command->path_components()[0] == "connections") {
    if (command->path_components().size() == 1) {
      au::tables::Table *table = getConnectionsTable();
      command->Append(table->strFormatted(command->format()));
      return;
    } else if (command->path_components().size() == 2) {
      au::tables::Table *table = getConnectionsTable(command->path_components()[1]);
      command->Append(table->strFormatted(command->format()));
      return;
    } else {
      command->AppendFormatedError("Wrong format. Only /connections/format is supported");
      return;
    }
  }


  if (command->path_components()[0] == "add_channel") {
    if (command->path_components().size() == 1) {
      command->AppendFormatedError("Wrong format. /add_channe/channel_name[/splitter]");
      return;
    }

    std::string channel_name = command->path_components()[1];
    std::string splitter_name;

    if (command->path_components().size() > 2)
      splitter_name = command->path_components()[2];

    au::ErrorManager error;
    process_command(au::str("add_channel %s %s", channel_name.c_str(), splitter_name.c_str()), &error);
    if (error.IsActivated())
      command->AppendFormatedError(error.GetMessage()); else
      command->AppendFormatedElement("Result", "OK");
    return;
  }

  if (( command->path_components()[0] == "add_input_adaptor" ) || ( command->path_components()[0] == "add_output_adaptor" )) {
    if (command->path_components().size() < 3) {
      command->AppendFormatedError("Wrong format. /add_[ input or output ]_adaptor/channel.adaptor_name/format");
      return;
    }

    std::string operation = command->path_components()[0];
    std::string name = command->path_components()[1];
    std::string format = command->path_components()[2];

    au::ErrorManager error;
    process_command(au::str("%s %s %s", operation.c_str(), name.c_str(), format.c_str()), &error);
    if (error.IsActivated())
      command->AppendFormatedError(error.GetMessage()); else
      command->AppendFormatedElement("Result", "OK");
    return;
  }
}

int StreamConnector::getNumInputItems() {
  au::TokenTaker tt(&token);
  int total = 0;

  au::map<std::string, Channel>::iterator it_channels;
  for (it_channels = channels_.begin(); it_channels != channels_.end(); it_channels++) {
    total += it_channels->second->getNumInputItems();
  }
  return total;
}

size_t StreamConnector::getOutputConnectionsBufferedSize() {
  au::TokenTaker tt(&token);
  size_t total = 0;

  au::map<std::string, Channel>::iterator it_channels;
  for (it_channels = channels_.begin(); it_channels != channels_.end(); it_channels++) {
    Channel *channel = it_channels->second;
    total += channel->getOutputConnectionsBufferedSize();
  }
  return total;
}

void StreamConnector::review() {
  au::TokenTaker tt(&token);

  // Review input inter channel connections
  au::list<InputInterChannelConnection>::iterator it;
  for (it = input_inter_channel_connections.begin(); it != input_inter_channel_connections.end(); it++) {
    InputInterChannelConnection *c = *it;
    if (c->is_finished()) {
      delete c;
      input_inter_channel_connections.erase(it);
    }
  }

  // Review all channels
  au::map<std::string, Channel>::iterator it_channels;
  for (it_channels = channels_.begin(); it_channels != channels_.end(); it_channels++) {
    it_channels->second->review();
  }
}

std::string StreamConnector::getPrompt() {
  return "SC console> ";
}

void StreamConnector::evalCommand(std::string command) {
  // Log activity
  Log("Console", "Message", command);

  au::ErrorManager error;
  process_command(command, &error);
  write(&error);
}

void StreamConnector::autoCompleteWithChannelNames(au::ConsoleAutoComplete *info) {
  // Mutex protection
  au::TokenTaker tt(&token);

  au::map<std::string, Channel>::iterator it;
  for (it = channels_.begin(); it != channels_.end(); it++) {
    info->add(it->first, it->first, false);
  }
}

void StreamConnector::autoCompleteWithAdaptorsNames(au::ConsoleAutoComplete *info) {
  // Mutex protection
  au::TokenTaker tt(&token);

  au::map<std::string, Channel>::iterator it;
  for (it = channels_.begin(); it != channels_.end(); it++) {
    Channel *channel = it->second;
    channel->autoCompleteWithAdaptorsNames(info);
  }
}

void StreamConnector::autoComplete(au::ConsoleAutoComplete *info) {
  if (info->completingFirstWord()) {
    info->add("quit");
    info->add("help");
    info->add("memory_status");

    info->add("services");
    info->add("logs");

    info->add("show_channels");
    info->add("show_adaptors");
    info->add("show_connections");

    info->add("show_input_connections");
    info->add("show_output_connections");

    info->add("add_channel");

    info->add("add_input_adaptor");
    info->add("add_output_adaptor");

    info->add("show_input_inter_channel_connections");

    info->add("remove_channel");
    info->add("remove_adapter");

    info->add("remove_finished_adaptors_and_connections");

    info->add("summary");
  }

  if (( info->completingSecondWord("add_input_adaptor"))
      || ( info->completingSecondWord("add_output_adaptor"))
      || ( info->completingSecondWord("remove_channel")))
    // Autocomplete with channel names
    autoCompleteWithChannelNames(info);

  if (info->completingSecondWord("show_channels"))
    info->add("-data");
  if (info->completingSecondWord("show_adaptors"))
    info->add("-data");
  if (info->completingSecondWord("show_connections"))
    info->add("-data");

  if (info->completingSecondWord("remove_adapter"))
    // Autocomplete with channel names
    autoCompleteWithAdaptorsNames(info);

  if (( info->firstWord() == "add_input_to_channel" ) && info->completingThirdWord()) {
    info->setHelpMessage("add_input_to_channel channel [port:8888] [connection:host:port] [samson:host:queue]");
    info->add("connection:", "connection:", false);
    info->add("port:", "port:", false);
    info->add("samson:", "samson:", false);
    info->add("inter_channel");
  }

  if (( info->firstWord() == "add_output_to_channel" ) && info->completingThirdWord()) {
    info->setHelpMessage("add_output [port:8888] [connection:host:port] [samson:host:queue]");
    info->add("connection:", "connection:", false);
    info->add("port:", "port:", false);
    info->add("samson:", "samson:", false);
  }
}

void StreamConnector::process_command(std::string command, au::ErrorManager *error) {
  // Mutex protection for whatever manipulation
  au::TokenTaker tt(&token);

  // Command line to be processed
  CommandLine cmdLine(command);

  if (cmdLine.get_num_arguments() == 0)
    return;

  // My commands
  std::string main_command = cmdLine.get_argument(0);

  if (main_command == "threads") {
    std::ostringstream message;
    message << *au::ThreadManager::shared();
    error->AddMessage(message.str());
    return;
  }

  if (main_command == "summary") {
    au::tables::Table *table =  getSummaryTable();
    error->AddMessage(table->str());
    delete table;
    return;
  }

  if (main_command == "logs") {
    size_t limits = 0;

    if (cmdLine.get_num_arguments() > 1)
      limits = atoi(cmdLine.get_argument(1).c_str());
    LogManager *log_manager = au::Singleton<LogManager>::shared();
    au::tables::Table *table = log_manager->getLogsTable(limits);

    error->AddMessage(table->str());
    delete table;

    return;
  }

  if (main_command == "services") {
    au::tables::Table table("Service|port|Status,left");

    {
      au::StringVector values("Remove console");
      values.Push(sc_console_port);

      if (service)
        values.push_back(service->GetStringStatus()); else
        values.push_back("NOT ACTIVATED");
      table.addRow(values);
    }

    {
      au::StringVector values("REST");
      values.Push(sc_web_port);

      if (rest_service)
        values.push_back(rest_service->GetStringStatus()); else
        values.push_back("NOT ACTIVATED");
      table.addRow(values);
    }


    {
      au::StringVector values("InterChannel");
      values.Push(SAMSON_CONNECTOR_INTERCHANNEL_PORT);

      if (inter_channel_listener) {
        if (inter_channel_listener->IsNetworkListenerRunning())
          values.push_back("Listening"); else
          values.push_back("Not Listening");
      } else {
        values.push_back("NOT ACTIVATED");
      }

      table.addRow(values);
    }


    error->AddMessage(table.str());
    return;
  }

  if (main_command == "help") {
    au::tables::Table table("Command|Description,left");

    table.addRow(au::StringVector("help", "Show this help"));
    table.addRow(au::StringVector("show_channels", "Show current channels"));
    table.addRow(au::StringVector("show_adaptors", "Show adaptors for all channels"));
    table.addRow(au::StringVector("show_connections", "Show all connections for all adaptors for all channels"));
    table.addRow(au::StringVector("show_input_connections", "Show all input connections"));
    table.addRow(au::StringVector("show_output_connections", "Show all output connections"));
    table.addRow(au::StringVector("add_input_adaptor_to_channel",
                                  "add_adaptor_input channel.adaptor_name [port:8888] [connection:host:port] [samson:host:queue]"));
    table.addRow(au::StringVector("add_output_adaptor_to_channel",
                                  "add_adaptor_output channel.adaptor_name [port:8888] [connection:host:port] [samson:host:queue]"));
    table.addRow(au::StringVector("remove_adaptor", "remove_adaptor channel.adaptor"));
    table.setTitle("Available commands for samsonConnector:");

    error->AddMessage(table.str());
    return;
  }

  if (main_command == "memory_status") {
    std::string message =  au::str("Used memory %s / %s "
                                   , au::str(engine::MemoryManager::shared()->used_memory()).c_str()
                                   , au::str(engine::MemoryManager::shared()->memory()).c_str()
                                   );
    error->AddMessage(message);
    return;
  }

  if (main_command == "show_channels") {
    std::string table_type = "default";

    if (cmdLine.isDataFlag())
      table_type = "data";
    au::tables::Table *table = getChannelsTable(table_type);
    error->AddMessage(table->str());
    delete table;
    return;
  }

  if (main_command == "show_adaptors") {
    std::string table_type = "default";

    if (cmdLine.isDataFlag())
      table_type = "data";

    au::tables::Table *table = getItemsTable(table_type);
    error->AddMessage(table->str());
    delete table;

    return;
  }

  if (main_command == "show_connections") {
    std::string table_type = "default";
    std::string select_channel = "";

    if (cmdLine.isDataFlag())
      table_type = "data";
    au::tables::Table *table = getConnectionsTable(table_type, select_channel);
    error->AddMessage(table->str());
    delete table;
    return;
  }

  if (main_command == "show_input_inter_channel_connections") {
    au::tables::Table *table = getInputInterChannelConnections();
    error->AddMessage(table->str());
    delete table;
    return;
  }

  // Log activity ( command that modify state )
  log(new Log("SamsonConnector", "Message", command));

  if (main_command == "add_channel") {
    if (cmdLine.get_num_arguments() < 2) {
      error->set("Usage: add_channel name splitter");
      return;
    }

    std::string name = cmdLine.get_argument(1);
    std::string splitter = "";
    if (cmdLine.get_num_arguments() > 2)
      splitter = cmdLine.get_argument(2);
    if (channels_.findInMap(name) != NULL) {
      error->set(au::str("Channel %s already exist", name.c_str()));
      return;
    }

    // Insert new channel
    channels_.insertInMap(name, new Channel(this, name, splitter));
    error->AddMessage(au::str("Channel %s added.", name.c_str()));
    return;
  }


  if (main_command == "add_input_adaptor") {
    if (cmdLine.get_num_arguments() < 3) {
      error->set("Usage: add_input_adaptor channel.name adapter_definition");
      return;
    }

    std::string full_name = cmdLine.get_argument(1);
    std::string adapter_definition = cmdLine.get_argument(2);


    // Split full name in channel.name
    std::vector<std::string> full_name_components = au::split(full_name, '.');
    if (full_name_components.size() != 2) {
      error->set("Usage: add_input_adaptor channel.name adapter_definition");
      return;
    }

    std::string channel_name = full_name_components[0];
    std::string name = full_name_components[1];


    Channel *channel = channels_.findInMap(channel_name);

    if (!channel) {
      error->set(au::str("Unknown channel %s", channel_name.c_str()));
      return;
    }

    channel->add_input(name, adapter_definition, error);

    if (!error->IsActivated())
      error->AddMessage(au::str("Input adapter %s.%s (%s) added correctly"
                                , channel_name.c_str()
                                , name.c_str()
                                , adapter_definition.c_str())); return;
  }

  if (main_command == "add_output_adaptor") {
    if (cmdLine.get_num_arguments() < 3) {
      error->set("Usage: add_output_adaptor channel.name adapter_definition");
      return;
    }

    std::string full_name = cmdLine.get_argument(1);
    std::string adapter_definition = cmdLine.get_argument(2);


    // Split full name in channel.name
    std::vector<std::string> full_name_components = au::split(full_name, '.');
    if (full_name_components.size() != 2) {
      error->set("Usage: add_output_adaptor channel.name adapter_definition");
      return;
    }

    std::string channel_name = full_name_components[0];
    std::string name = full_name_components[1];

    Channel *channel = channels_.findInMap(channel_name);

    if (!channel) {
      error->set(au::str("Unknown channel %s", channel_name.c_str()));
      return;
    }

    channel->add_output(name, adapter_definition, error);
    if (!error->IsActivated())
      error->AddMessage(au::str("Output adapter %s.%s (%s) added correctly"
                                , channel_name.c_str()
                                , name.c_str()
                                , adapter_definition.c_str())); return;
  }

  if (main_command == "remove_channel") {
    if (cmdLine.get_num_arguments() < 2) {
      error->set("Usage: remove_channel channel_name");
      return;
    }

    std::string channel_name = cmdLine.get_argument(1);

    Channel *channel = channels_.extractFromMap(channel_name);

    if (!channel) {
      error->set(au::str("Unknown channel %s", channel_name.c_str()));
      return;
    }

    // Remove the entire channel
    channel->cancel_channel();
    delete channel;

    if (!error->IsActivated())
      error->AddMessage(au::str("Channel %s removed correclty", channel_name.c_str()));
    return;
  }

  if (main_command == "remove_adapter") {
    if (cmdLine.get_num_arguments() < 2) {
      error->set("Usage: remove_adaptor channel.adapter");
      return;
    }

    std::string full_name = cmdLine.get_argument(1);
    std::vector<std::string> full_path_components = au::split(full_name, '.');

    if (( full_path_components.size() == 0 ) || ( full_path_components.size() > 2 )) {
      error->set("Usage: remove_adaptor channel.adapter");
      return;
    }

    std::string channel_name = full_path_components[0];

    Channel *channel = channels_.findInMap(channel_name);

    if (!channel) {
      error->set(au::str("Unknown channel %s", channel_name.c_str()));
      return;
    }

    channel->remove_item(full_path_components[1], error);

    if (!error->IsActivated())
      error->AddMessage(au::str("Adaptor %s removed correclty", full_name.c_str())); return;
  }

  if (main_command == "remove_finished_adaptors_and_connections") {
    au::ErrorManager error;
    remove_finished_items_and_connections(&error);
    write(&error);
    return;
  }

  // Unknown command error
  error->set(au::str("Unknown command %s", main_command.c_str()));
}

void StreamConnector::remove_finished_items_and_connections(au::ErrorManager *error) {
  au::TokenTaker tt(&token);

  // Review all channels
  au::map<std::string, Channel>::iterator it_channels;
  for (it_channels = channels_.begin(); it_channels != channels_.end(); it_channels++) {
    Channel *channel = it_channels->second;
    channel->remove_finished_items_and_connections(error);
  }
}

au::tables::Table *StreamConnector::getChannelsTable(std::string type) {
  std::string fields;

  if (type == "data")
    fields = "Channel,left|#adaptors|#connections|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s"; else
    fields = "Channel,left|Inputs|splitter,left|Outputs,left";
  au::tables::Table *table = new au::tables::Table(fields);
  table->setTitle("Channels");

  // Review all channels
  au::map<std::string, Channel>::iterator it_channels;
  for (it_channels = channels_.begin(); it_channels != channels_.end(); it_channels++) {
    Channel *channel = it_channels->second;

    au::StringVector values;

    // Common fields
    values.push_back(it_channels->first);


    if (type == "data") {
      values.push_back(au::str("%d", channel->getNumItems()));
      values.push_back(au::str("%d", channel->getNumConnections()));

      values.push_back(au::str(channel->traffic_statistics.get_input_total()));
      values.push_back(au::str(channel->traffic_statistics.get_input_rate()));
      values.push_back(au::str(channel->traffic_statistics.get_output_total()));
      values.push_back(au::str(channel->traffic_statistics.get_output_rate()));
    } else {
      values.push_back(channel->getInputsString());
      values.push_back(channel->getSplitter());
      values.push_back(channel->getOutputsString());
    }

    table->addRow(values);
  }

  return table;
}

au::tables::Table *StreamConnector::getInputInterChannelConnections() {
  au::TokenTaker tt(&token);

  au::tables::Table *table = new au::tables::Table("Host&Port|Status,left");

  table->setTitle("Input InterChannel connections");

  au::list<InputInterChannelConnection>::iterator it;
  for (it = input_inter_channel_connections.begin(); it != input_inter_channel_connections.end(); it++) {
    InputInterChannelConnection *c = *it;
    table->addRow(au::StringVector(c->host_and_port(), c->getStatus()));
  }
  return table;
}

au::tables::Table *StreamConnector::getConnectionsTable(std::string type, std::string select_channel) {
  au::TokenTaker tt(&token);

  std::string fields;

  if (type == "data")
    fields = "Name,left|Type,left|Description,left|Buffer|Buffer (mem)|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s"; else
    fields = "Name,left|Type,left|Description,left|Finish|Status,left|Info,left";

  au::tables::Table *table = new au::tables::Table(fields);
  table->setTitle("Connections");

  Channel *previous_channel = NULL;   // Used to draw separation lines in the table

  au::map<std::string, Channel>::iterator it_channels;
  for (it_channels = channels_.begin(); it_channels != channels_.end(); it_channels++) {
    Channel *channel = it_channels->second;

    // Lock the token in the channel
    au::TokenTaker tt(&channel->token);


    au::map<std::string, Adaptor>::iterator it_items;
    for (it_items = channel->items.begin(); it_items != channel->items.end(); it_items++) {
      Adaptor *item = it_items->second;

      // Lock the token in the item
      au::TokenTaker tt(&item->token);

      au::map<int, Connection>::iterator it_connections;
      for (it_connections = item->connections.begin(); it_connections != item->connections.end(); it_connections++) {
        Connection *connection = it_connections->second;

        // Draw separation line if necessary
        if (( previous_channel ) && ( previous_channel != channel ))
          table->addSeparator(); previous_channel = channel;

        // Get the values for the connections
        au::StringVector values;

        // Common fields
        values.push_back(connection->getFullName());
        values.push_back(connection->getTypeStr());
        values.push_back(connection->getDescription());

        // Specific fields
        if (type == "data") {
          // Buffer information
          values.push_back(au::str(connection->bufferedSize(), "B"));
          values.push_back(au::str(connection->bufferedSizeOnMemory(), "B"));

          values.push_back(au::str(connection->traffic_statistics.get_input_total()));
          values.push_back(au::str(connection->traffic_statistics.get_input_rate()));
          values.push_back(au::str(connection->traffic_statistics.get_output_total()));
          values.push_back(au::str(connection->traffic_statistics.get_output_rate()));
        } else {
          if (connection->is_finished())
            values.push_back("F"); else
            values.push_back("");
          values.push_back(connection->getConnectionStatus());
          values.push_back(connection->getStatus());
        }

        table->addRow(values);
      }
    }
  }

  return table;
}

au::tables::Table *StreamConnector::getSummaryTable() {
  au::TokenTaker tt(&token);

  au::tables::Table *table = new au::tables::Table("Concept|Value,left");

  table->setTitle("Summary");


  table->addRow(au::StringVector("Active time", au::S(cronometer)));

  table->addRow(au::StringVector("Total input", au::str(traffic_statistics.get_input_total(), "B")));
  table->addRow(au::StringVector("Total output", au::str(traffic_statistics.get_output_total(), "B")));

  table->addRow(au::StringVector("Input rate", au::str(traffic_statistics.get_input_rate(), "B")));
  table->addRow(au::StringVector("Output rate", au::str(traffic_statistics.get_output_rate(), "B")));

  return table;
}

au::tables::Table *StreamConnector::getItemsTable(std::string type) {
  au::TokenTaker tt(&token);

  std::string fields;

  if (type == "data")
    fields = "Name,left|Type,left|Description,left|#connections|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s"; else
    fields = "Name,left|Type,left|Description,left|Finish|Status";
  au::tables::Table *table = new au::tables::Table(fields);
  table->setTitle("Adaptors");

  au::map<std::string, Channel>::iterator it_channels;
  for (it_channels = channels_.begin(); it_channels != channels_.end(); ) {
    Channel *channel = it_channels->second;

    // Lock the token in the channel
    au::TokenTaker tt(&channel->token);

    au::map<std::string, Adaptor>::iterator it_items;
    for (it_items = channel->items.begin(); it_items != channel->items.end(); it_items++) {
      Adaptor *item = it_items->second;


      au::StringVector values;
      values.push_back(item->getFullName());
      values.push_back(item->getTypeStr());
      values.push_back(item->getDescription());

      if (type == "data") {
        values.push_back(au::str("%d", item->getNumConnections()));
        values.push_back(au::str(item->traffic_statistics.get_input_total()));
        values.push_back(au::str(item->traffic_statistics.get_input_rate()));
        values.push_back(au::str(item->traffic_statistics.get_output_total()));
        values.push_back(au::str(item->traffic_statistics.get_output_rate()));
      } else {
        if (item->is_finished())
          values.push_back("F"); else
          values.push_back("");
        values.push_back(item->getStatus());
      }



      table->addRow(values);
    }

    it_channels++;

    // Write a separator in the table if this is not the last line of the table
    if (it_channels !=  channels_.end())
      if (channel->items.size() > 0)
        table->addSeparator();
  }

  return table;
}

void StreamConnector::log(Log *log) {
  au::TokenTaker tt(&token);

  // Create au::ErrorManager element
  au::ErrorManager error;

  std::string type = log->getType();

  if (type == "Warning")
    error.AddWarning(log->getNameAndMessage()); else if (type == "Error")
    error.AddError(log->getNameAndMessage()); else
    error.AddMessage(log->getNameAndMessage());
  if (interactive) {
    au::Console::write(&error);
  } else if (run_as_daemon) {
    // Nothing here
  } else {
    // Print on screen
    if (paVerbose)
      std::cerr << error.str();
  }


  // Add to the log system
  LogManager *log_manager = au::Singleton<LogManager>::shared();
  log_manager->log(log);
}

void StreamConnector::newSocketConnection(au::NetworkListener *listener
                                          , au::SocketConnection *socket_connetion) {
  // Create a new connection
  InputInterChannelConnection *connection = new InputInterChannelConnection(this, socket_connetion->host_and_port(), socket_connetion);

  // We start connection here
  connection->init_connecton();

  // Push into the list of connections
  input_inter_channel_connections.push_back(connection);
}

void StreamConnector::select_channel(InputInterChannelConnection *connection, std::string target_channel, au::ErrorManager *error) {
  au::TokenTaker tt(&token);

  Channel *channel = channels_.findInMap(target_channel);

  if (!channel) {
    error->set(au::str("Channel %s not found", target_channel.c_str()));
    return;
  }

  au::TokenTaker tt2(&channel->token);

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = channel->items.begin(); it_items != channel->items.end(); it_items++) {
    Adaptor *item = it_items->second;

    // Check if this item can receive this connection
    if (item->accept(connection)) {
      // Set this item as connection's item
      connection->item = item;

      // Extract from the list of pending connections
      input_inter_channel_connections.extractFromList(connection);

      // Inser in selected item
      item->add(connection);

      return;
    }
  }

  error->set(au::str("Channel %s do not accept this connection", target_channel.c_str()));
  return;
}

std::string StreamConnector::getPasswordForUser(std::string user) {
  au::TokenTaker tt(&token);

  return environment.get(au::str("user_%s", user.c_str()), "");
}

void StreamConnector::setPasswordForUser(std::string user, std::string password) {
  au::TokenTaker tt(&token);

  environment.set(au::str("user_%s", user.c_str()), password);
}

void StreamConnector::report_output_size(size_t size) {
  traffic_statistics.push_output(size);
}

void StreamConnector::report_input_size(size_t size) {
  traffic_statistics.push_input(size);
}
}
