
#include "Adaptor.h"
#include "Channel.h"  // Own interface
#include "ChannelAdaptor.h"
#include "DiskAdaptor.h"
#include "HDFSAdaptor.h"
#include "ListenerAdaptor.h"
#include "SamsonAdaptor.h"
#include "ServerConnection.h"
#include "StreamConnector.h"
#include "common.h"

extern bool interactive;
extern bool run_as_daemon;

namespace stream_connector {
Channel::Channel(StreamConnector *connector, std::string name, std::string splitter) : token("token_Channel") {
  // Keep name and pointer to connector
  connector_ = connector;
  name_ = name;
  splitter_ = splitter;
}

Channel::~Channel() {
  cancel_channel();
  items.clearMap();
}

void Channel::remove_finished_items_and_connections(au::ErrorManager *error) {
  // Mutex protection
  au::TokenTaker tt(&token);

  // ------------------------------------------------------------
  // Review all items
  // ------------------------------------------------------------

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    Adaptor *item = it_items->second;

    // Remove finished connections
    item->remove_finished_connections(error);

    if ((item->getNumConnections() == 0 ) && ( item->is_finished())) {
      log("Message", au::str("Removing adaptor %s", item->getFullName().c_str()));

      item->cancel_item();
      delete item;
      items.erase(it_items);
    }
  }
}

void Channel::review() {
  // Mutex protection
  au::TokenTaker tt(&token);

  // ------------------------------------------------------------
  // Review all items
  // ------------------------------------------------------------

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    Adaptor *item = it_items->second;
    item->review();
  }
}

void Channel::add_output(std::string name, std::string output_string, au::ErrorManager *error) {
  // Mutex protection
  au::TokenTaker tt(&token);

  Adaptor *previous_item = items.findInMap(name);

  if (previous_item) {
    error->set(au::str("Item %s already exist (%s)", previous_item->getFullName().c_str(),
                       previous_item->getDescription().c_str()));
    return;
  }

  std::vector<std::string> components = au::split(output_string, ':');

  if (components[0] == "stdout") {
    if (!interactive && !run_as_daemon)
      add(name,  new StdoutItem(this)); else
      // Non using stdout in interactive mode
      return;
  } else if (components[0] == "port") {
    if (components.size() < 2) {
      error->set("Output port without number. Please specifiy port to open (ex port:10000)");
      return;
    }

    int port = atoi(components[1].c_str());
    if (port == 0) {
      error->set("Wrong output port");
      return;
    }

    // Add a listen item
    add(name, new ListenerAdaptor(this, connection_output, port));
    error->AddMessage(au::str("Added an output item to channel %s listening plain socket connection on port %d ",
                              name_.c_str(), port));
    return;
  } else if (components[0] == "disk") {
    if (components.size() < 2) {
      error->set("Usage disk:file_name_or_dir");
      return;
    }

    add(name, new DiskAdaptor(this, connection_output, components[1]));
  } else if (components[0] == "connection") {
    if (components.size() < 3) {
      error->set("Output connection without host and port. Please specifiy as connection:host:port)");
      return;
    }

    std::string host = components[1];
    int port = atoi(components[2].c_str());
    if (port == 0) {
      error->set(au::str("Wrong connection port for %s", host.c_str()));
      return;
    }

    add(name,  new ConnectionItem(this, connection_output, host, port));
  } else if (components[0] == "samson") {
    std::string host = "localhost";
    std::string queue = "input";
    int port = SAMSON_WORKER_PORT;

    if (components.size() == 2) {
      queue = components[1];
    } else if (components.size() == 3) {
      host = components[1];
      queue = components[2];
    } else if (components.size() >= 3) {
      host = components[1];
      port = atoi(components[2].c_str());
      queue = components[3];
    }

    add(name,  new SamsonAdaptor(this, connection_output, host, port, queue));
  } else if (components[0] == "channel") {
    if (components.size() < 3) {
      error->set("Output connection without host and channel. Please specifiy as channel:host:channel)");
      return;
    }

    std::string host = components[1];
    std::string channel = components[2];

    add(name,  new OutputChannelAdaptor(this, host, channel));
  } else if (components[0] == "hdfs") {
    if (components.size() < 3) {
      error->set("Wrong format: hdfs:host:directory");
      return;
    }

    std::string host = components[1];
    std::string directory = components[2];
    add(name, new HDFSAdaptor(this, connection_output, host, directory));
  } else {
    // Error message
    error->AddError(au::str("Unknown output definition %s", components[0].c_str()));
  }
}

void Channel::add_input(std::string name, std::string input_string, au::ErrorManager *error) {
  // Mutex protection
  au::TokenTaker tt(&token);

  Adaptor *previous_item = items.findInMap(name);

  if (previous_item) {
    error->set(au::str("Item %s already exist (%s)", name.c_str(), previous_item->getDescription().c_str()));
    return;
  }

  std::vector<std::string> components = au::split(input_string, ':');

  if (components[0] == "stdin") {
    if (!interactive && !run_as_daemon) {        // No send to add stdout in interactive mode ;)
      add(name, new StdinItem(this));
    } else {
      // Not adding stdin in interactive mode
      error->AddError("stdin is not available in daemon mode");
      return;
    }
  } else if (components[0] == "port") {
    if (components.size() < 2) {
      error->set("Input port without number. Please specifiy port to open (ex port:10000)");
      return;
    }

    int port = atoi(components[1].c_str());
    if (port == 0) {
      error->set("Wrong input port");
      return;
    }

    // Add a listen item
    add(name, new ListenerAdaptor(this, connection_input, port));
    error->AddMessage(au::str("Added an input item to channel %s listening plain socket connection on port %d ",
                              name_.c_str(), port));
    return;
  } else if (components[0] == "disk") {
    if (components.size() < 2) {
      error->set("Usage disk:file_name_or_dir");
      return;
    }

    add(name, new DiskAdaptor(this, connection_input, components[1]));
  } else if (components[0] == "connection") {
    if (components.size() < 3) {
      error->set("Input connection without host and port. Please specifiy as connection:host:port)");
      return;
    }

    std::string host = components[1];
    int port = atoi(components[2].c_str());
    if (port == 0) {
      error->set(au::str("Wrong connection port for %s", host.c_str()));
      return;
    }

    add(name, new ConnectionItem(this, connection_input, host, port));
  } else if (components[0] == "samson") {
    std::string host = "localhost";
    std::string queue = "input";
    int port = SAMSON_WORKER_PORT;

    if (components.size() == 2) {
      queue = components[1];
    } else if (components.size() == 3) {
      host = components[1];
      queue = components[2];
    } else if (components.size() >= 3) {
      host = components[1];
      port = atoi(components[2].c_str());
      queue = components[3];
    }

    add(name, new SamsonAdaptor(this, connection_input, host, port, queue));
  } else if (components[0] == "channel") {
    // Able to receive connections for inter-channel connection
    add(name, new InputChannelAdaptor(this));
  } else if (components[0] == "hdfs") {
    if (components.size() < 3) {
      error->set("Wrong format: hdfs:host:directory");
      return;
    }

    std::string host = components[1];
    std::string directory = components[2];
    add(name, new HDFSAdaptor(this, connection_input, host, directory));
  } else {
    // Error message
    error->AddError(au::str("Unknown input definition %s", components[0].c_str()));
  }
}

// Generic method to add an item
void Channel::add(std::string name, Adaptor *item) {
  // Mutex protection
  au::TokenTaker tt(&token);

  if (!item)
    return;

  // Set name of this connection
  item->name_ = name;

  // Insert in the map of items
  items.insertInMap(name, item);

  // Log activity
  log("Message", au::str("Adaptor %s (%s) added"
                         , item->getFullName().c_str()
                         , item->getDescription().c_str()));

  // Init the item
  item->init_item();
}

void Channel::remove_item(std::string name, au::ErrorManager *error) {
  // Mutex protection
  au::TokenTaker tt(&token);

  Adaptor *item = items.extractFromMap(name);

  if (!item) {
    error->set(au::str("Adaptor %s.%s not found", getName().c_str(), name.c_str()));
    return;
  }

  // Cancel item
  item->cancel_item();
  delete item;
}

void Channel::push(engine::BufferPointer buffer) {
  // Mutex protection
  au::TokenTaker tt(&token);

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    if (it_items->second->getType() == connection_output)
      it_items->second->push(buffer);
  }
}

int Channel::getNumItems() {
  // Mutex protection
  au::TokenTaker tt(&token);

  return items.size();
}

int Channel::getNumOutputItems() {
  // Mutex protection
  au::TokenTaker tt(&token);
  int total = 0;

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    if (it_items->second->getType() == connection_output)
      total++;
  }
  return total;
}

int Channel::getNumInputItems() {
  // Mutex protection
  au::TokenTaker tt(&token);
  int total = 0;

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    if (it_items->second->getType() == connection_input)
      total++;
  }
  return total;
}

int Channel::getNumConnections() {
  // Mutex protection
  au::TokenTaker tt(&token);
  int total = 0;

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    total += it_items->second->getNumConnections();
  }
  return total;
}

std::string Channel::getName() {
  return name_;
}

std::string Channel::getSplitter() {
  return splitter_;
}

std::string Channel::getInputsString() {
  std::ostringstream output;

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    Adaptor *item = it_items->second;
    if (item->getType() == connection_input)
      output << item->getDescription()  << " ";
  }
  return output.str();
}

void Channel::cancel_channel() {
  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    Adaptor *item = it_items->second;
    item->cancel_item();
  }
}

std::string Channel::getOutputsString() {
  std::ostringstream output;

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    Adaptor *item = it_items->second;
    if (item->getType() == connection_output)
      output << item->getDescription() << " ";
  }
  return output.str();
}

size_t Channel::getOutputConnectionsBufferedSize() {
  au::TokenTaker tt(&token);

  size_t total = 0;

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    Adaptor *item = it_items->second;

    if (item->getType() == connection_output)
      total += item->getConnectionsBufferedSize();
  }
  return total;
}

void Channel::log(std::string type, std::string message) {
  log(new Log(getName(), type, message));
}

void Channel::log(Log *log) {
  connector_->log(log);
}

void Channel::report_output_size(size_t size) {
  traffic_statistics.push_output(size);
  connector_->report_output_size(size);
}

void Channel::report_input_size(size_t size) {
  traffic_statistics.push_input(size);
  connector_->report_input_size(size);
}

void Channel::autoCompleteWithAdaptorsNames(au::ConsoleAutoComplete *info) {
  au::TokenTaker tt(&token);

  au::map<std::string, Adaptor>::iterator it_items;
  for (it_items = items.begin(); it_items != items.end(); it_items++) {
    Adaptor *item = it_items->second;
    info->add(item->getFullName());
  }
}
}
